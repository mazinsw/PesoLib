#include "CommPort.h"
#include "Thread.h"
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct CommPort
{
	HANDLE hFile;
	Event* cancel;
	CommInfo info;
};

CommPort* CommPort_create(const char* port)
{
	CommSettings settings;
	settings.baund = 9600;
	settings.data = 8;
	settings.stop = StopBits_One;
	settings.parity = Parity_None;
	settings.flow = Flow_None;
	return CommPort_createEx(port, &settings);
}

CommPort* CommPort_createEx(const char* port, const CommSettings* settings)
{
	char file[64];
	strcpy(file, "\\\\.\\");
	strcat(file, port);
	HANDLE hFile = CreateFile(file, GENERIC_READ | GENERIC_WRITE, 0, NULL,
							  OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);
	if(hFile == INVALID_HANDLE_VALUE)
	{
#ifdef DEBUGLIB
		printf("Failed to open port \"%s\"\n", port);
#endif
		return NULL;
	}
	/*if(!SetCommMask(hFile, EV_RXCHAR | EV_ERR))
	{
		CloseHandle(hFile);
#ifdef DEBUGLIB
		printf("Failed set comm mask\n");
#endif
		return NULL;
	}*/
	CommPort* comm = (CommPort*)malloc(sizeof(CommPort));
	comm->hFile = hFile;
	comm->cancel = Event_createEx(FALSE, FALSE);
	strcpy(comm->info.port, port);
	CommPort_configure(comm, settings);
	return comm;
}

int CommPort_configure(CommPort* comm, const CommSettings* settings)
{
	DCB dcb;
    COMMTIMEOUTS timeouts;
	
	memset(&dcb, 0, sizeof(DCB));
	dcb.DCBlength = sizeof(DCB);
	if(!GetCommState(comm->hFile, &dcb))
	{
#ifdef DEBUGLIB
		printf("Failed to get comm state\n");
#endif
		return 0;
	}
	/*
	 * Boilerplate.
	 */
	dcb.fBinary = TRUE;
	dcb.fDtrControl = DTR_CONTROL_ENABLE;
	dcb.fDsrSensitivity = FALSE;
	dcb.fTXContinueOnXoff = FALSE;
	dcb.fOutX = FALSE;
	dcb.fInX = FALSE;
	dcb.fErrorChar = FALSE;
	dcb.fNull = FALSE;
	dcb.fRtsControl = RTS_CONTROL_ENABLE;
	dcb.fAbortOnError = FALSE;
	dcb.fOutxCtsFlow = FALSE;
	dcb.fOutxDsrFlow = FALSE;
	
	dcb.BaudRate = settings->baund;
	dcb.ByteSize = settings->data;
	dcb.Parity = settings->parity;
	dcb.StopBits = settings->stop;
	if(settings->flow == Flow_XONXOFF)
	    dcb.fOutX = dcb.fInX = TRUE;
	else if(settings->flow == Flow_RTSCTS)
	{	    dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
	    dcb.fOutxCtsFlow = TRUE;	}
	else if(settings->flow == Flow_DSRDTR)
	{
	    dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;
	    dcb.fOutxDsrFlow = TRUE;
	}
	if(!SetCommState(comm->hFile, &dcb))
	{
#ifdef DEBUGLIB
		printf("Failed to set comm state\n");
#endif
		return 0;
	}	
	if (!GetCommTimeouts(comm->hFile, &timeouts))
	{
#ifdef DEBUGLIB
		printf("Failed to get comm timeouts\n");
#endif
		return 0;
	}	
	timeouts.ReadIntervalTimeout = 1;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = 0;
	timeouts.WriteTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 0;
	if (!SetCommTimeouts(comm->hFile, &timeouts))
	{
#ifdef DEBUGLIB
		printf("Failed to set comm timeouts\n");
#endif
		return 0;
	}
	comm->info.settings.baund = settings->baund;
	comm->info.settings.data = settings->data;
	comm->info.settings.stop = settings->stop;
	comm->info.settings.parity = settings->parity;
	comm->info.settings.flow = settings->flow;
	return 1;
}


int CommPort_enum(char * buffer, int size)
{
	DWORD BytesNeeded, Returned, I, count = 0;
	int Success, len;
	BYTE * PortsPtr;
	PPORT_INFO_1 InfoPtr;
	char * TempStr, * ptr, *search;
	Success = EnumPorts(NULL, 1, NULL, 0, &BytesNeeded, &Returned);
	if(Success || GetLastError() != ERROR_INSUFFICIENT_BUFFER)
	{
#ifdef DEBUGLIB
		printf("Failed to calculate buffer for enum ports\n");
#endif
		return 0;
	}	
	PortsPtr = (BYTE*)malloc(BytesNeeded);
	Success = EnumPorts(NULL, 1, PortsPtr, BytesNeeded, &BytesNeeded, &Returned);
	if(!Success)
	{
#ifdef DEBUGLIB
		printf("Failed to enum ports\n");
#endif
		return 0;
	}
	if(buffer == NULL || size < (int)(BytesNeeded + Returned + 1))
	{
		free(PortsPtr);
		return (BytesNeeded + Returned + 1) - size;
	}
	ptr = buffer;
	ptr[0] = 0;
	ptr[1] = 0;
	for(I = 0; I < Returned; I++)
	{
		InfoPtr = (PPORT_INFO_1)(PortsPtr + I * sizeof(PORT_INFO_1));
		TempStr = InfoPtr->pName;
		if(strstr(TempStr, "COM") != TempStr)
			continue;
		len = strlen(TempStr);
		search = strstr(TempStr, ":");
		if(search != NULL)
			len = search - TempStr;
		strncpy(ptr, TempStr, len);
		ptr[len] = 0;
		ptr += len;
		ptr[0] = 0;
		ptr[1] = 0;
		ptr++;
		count++;
	}
	free(PortsPtr);
	return count;
}

int CommPort_write(CommPort* comm, const unsigned char* bytes, int count)
{
	return CommPort_writeEx(comm, bytes, count, INFINITE);
}

int CommPort_wait(CommPort* comm, int* bytesAvailable)
{
	return CommPort_waitEx(comm, bytesAvailable, INFINITE);
}

int CommPort_read(CommPort* comm, unsigned char* bytes, int count)
{
	return CommPort_readEx(comm, bytes, count, INFINITE);
}

#ifdef DEBUGLIB
void Error(LPTSTR lpszFunction) 
{ 
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and exit the process

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR)); 
    snprintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"), 
        lpszFunction, (int)dw, (char*)lpMsgBuf); 
    printf("%s\n", (LPCTSTR)lpDisplayBuf);
//    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
}
#endif

int _CommPort_check(CommPort* comm, Event * evReceive, OVERLAPPED * ovl, 
	BOOL success, DWORD * bytesTrans, int milliseconds)
{
	Event* events[2];
	Event* signaled;
	
	events[0] = comm->cancel;
	events[1] = evReceive;
	if(success)
		return 1;
	if(GetLastError() != ERROR_IO_PENDING)
	{
#ifdef DEBUGLIB
		Error("_CommPort_check[WriteFile||ReadFile||WaitCommEvent]");
#endif
		return 0;
	}
	signaled = Event_waitMultipleEx(events, 2, milliseconds);
	if(signaled != evReceive)
	{
#ifdef DEBUGLIB
		Error("_CommPort_check[Event_waitMultipleEx]");
		if(signaled == NULL)
		{
			printf("Tempo limite excedido\n");
		}
		else
			printf("Acao cancelada\n");
#endif
		// cancel write operation to local variable
		CancelIo(comm->hFile);
		if(signaled == comm->cancel)
			Event_wait(evReceive); // wait for release local variable
		return 0;
	}
	if(!GetOverlappedResult(comm->hFile, ovl, bytesTrans, FALSE))
	{
#ifdef DEBUGLIB
		Error("_CommPort_check[GetOverlappedResult]");
#endif
		return 0;
	}
	return 1;
}

int CommPort_writeEx(CommPort* comm, const unsigned char* bytes, int count,
					int milliseconds)
{
	DWORD bytesTrans;
	OVERLAPPED ovl;
	Event * evReceive;

	evReceive = Event_createEx(TRUE, FALSE);
	memset(&ovl, 0, sizeof(OVERLAPPED));
	ovl.hEvent = (HANDLE)Event_getHandle(evReceive);
	BOOL success = WriteFile(comm->hFile, (LPCVOID)bytes, count, &bytesTrans, &ovl);
	success = _CommPort_check(comm, evReceive, &ovl, success, &bytesTrans, milliseconds);
	Event_free(evReceive);
	if(!success || bytesTrans != (DWORD)count)	
	{
#ifdef DEBUGLIB
		if(success)
			printf("Error: Write only %d of %d bytes\n", (int)bytesTrans, count);
#endif	
		return 0;
	}
	return bytesTrans;
}

int CommPort_waitEx(CommPort* comm, int* bytesAvailable, int milliseconds)
{
	DWORD erros;
	DWORD mask;
	DWORD bytesTrans;
	COMSTAT comStat;
	OVERLAPPED ovl;
	Event * evReceive;

	evReceive = Event_createEx(TRUE, FALSE);
	memset(&ovl, 0, sizeof(OVERLAPPED));
	ovl.hEvent = (HANDLE)Event_getHandle(evReceive);
	BOOL success = WaitCommEvent(comm->hFile, &mask, &ovl);
	success = _CommPort_check(comm, evReceive, &ovl, success, &bytesTrans, milliseconds);
	Event_free(evReceive);
	if(!success)
		return 0;
	// bytesTrans is count of event bytes, so get correct byte count in queue
	if(!ClearCommError(comm->hFile, &erros, &comStat))
	{
#ifdef DEBUGLIB
		Error("CommPort_waitEx[ClearCommError]");
#endif
		return 0;
	}
	*bytesAvailable = comStat.cbInQue;
	return 1;
}

int CommPort_readEx(CommPort* comm, unsigned char* bytes, int count, int milliseconds)
{
	DWORD bytesTrans;
	OVERLAPPED ovl;
	Event * evReceive;

	evReceive = Event_createEx(TRUE, FALSE);
	memset(&ovl, 0, sizeof(OVERLAPPED));
	ovl.hEvent = (HANDLE)Event_getHandle(evReceive);
	BOOL success = ReadFile(comm->hFile, (PVOID)bytes, count, NULL, &ovl);
	success = _CommPort_check(comm, evReceive, &ovl, success, &bytesTrans, milliseconds);
	Event_free(evReceive);
	if(!success || bytesTrans == (DWORD)-1)
		return 0;
#ifdef DEBUGLIB
	if(bytesTrans > count) {
		printf("CommPort_readEx[ReadCountError]: Trying to read %d bytes, but %li read instead\n", 
			count, bytesTrans);
	}#endif
	return bytesTrans;
}

void CommPort_cancel(CommPort* comm)
{
	Event_post(comm->cancel);
}

void CommPort_free(CommPort* comm)
{
	CommPort_cancel(comm);
	Event_free(comm->cancel);
	CloseHandle(comm->hFile);
	free(comm);
}
