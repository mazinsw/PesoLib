#include <PesoLib.h>
#include <stdio.h>
#include "Thread.h"
#include "Mutex.h"
#include "Event.h"
#include "CommPort.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "DeviceManager.h"
#include "StringBuilder.h"
#include "Mutex.h"
#define PROPERY_BUFFER_SIZE 256
static DeviceManager * gDevMan = NULL;
static const char gVersion[] = "1.2.0.0";
static const int baundValues[] = {9600, 2400, 4800, 14400, 19200, 38400, 57600, 115200, 230400, 460800, 921600, 110, 300, 600, 1200};

struct PesoLib
{
	int canceled;
	Thread* thConnect;
	Thread* thReceive;
	Event* evCancel;
	Event* evReceive;
	Event* evConnect;
	CommPort * comm;
	int deviceIndex;
	int baundIndex;
	float price;
	int weight;
	unsigned char stable;
	int connectTimeout;
	int retryTimeout;
	int requestAliveInterval;
	char port[8];
	CommSettings commSettings;
	StringBuilder* config;
	Mutex* mutex;
	Mutex* writeMutex;
};

static int _PesoLib_echoTest(PesoLib * lib, CommPort* comm, int testCount);

static void _PesoLib_reconnect(PesoLib * lib)
{
#ifdef DEBUGLIB
	printf("disconnecting device\n");
#endif
	CommPort_free(lib->comm);
#ifdef DEBUGLIB
	printf("comm port freed\n");
#endif
	lib->comm = NULL;
	Event_post(lib->evConnect);
#ifdef DEBUGLIB
	printf("sent disconnect event\n");
#endif
	lib->canceled = 1;
	Thread_start(lib->thConnect); // try connect again
}

static int _PesoLib_dataReceived(PesoLib * lib, const unsigned char * buffer, 
	int size, int deviceIndex, int testCount)
{
	int count = 0, consummed = 0, save_consummed;
	const char * val;
#ifdef DEBUGLIB
	printf("Data received %s\n", buffer);
#endif
	Device * dev = DeviceManager_getDevice(gDevMan, deviceIndex);
	while(1)
	{
		consummed = Device_test(dev, buffer, size);
		if(!consummed)
#ifdef DEBUGLIB
		{
			printf("Incompatible device driver\n");
#endif
			return count;
#ifdef DEBUGLIB
		}
		printf("Using %s driver\n", Device_getProperty(dev, "name"));
#endif
		save_consummed = consummed;
		consummed = Device_execute(dev, buffer, size);
		val = NULL;
		if(consummed)
		{
			val = Device_getProperty(dev, "weight");
			if(val == NULL) // property not found
				return count;
		}
		else
			consummed = save_consummed; // restore unstable buffer size
		if(testCount > 0)
			testCount--;
		else if(val != NULL) // stable weight
		{
			lib->weight = atoi(val);
			Event_post(lib->evReceive);
		}
		count++;
		buffer += consummed;
		size -= consummed;
		if(size <= 0)
			break;
	}
	return count;
}

static void _PesoLib_receiveFunc(void* data)
{
	PesoLib * lib = (PesoLib*)data;
	int bytesAvailable, bytesSaved = 0;
#ifdef DEBUGLIB
	printf("waiting for receive event\n");
#endif
	unsigned char * savedBuff = (unsigned char *)malloc(sizeof(unsigned char));
	Device * dev = DeviceManager_getDevice(gDevMan, lib->deviceIndex);
	const char * csz = Device_getProperty(dev, "response_size");
	int sz = (csz == NULL? 0: atoi(csz));
	Mutex_lock(lib->mutex);
	while(lib->canceled == 0)
	{
		if(CommPort_waitEx(lib->comm, &bytesAvailable, lib->requestAliveInterval))
		{
#ifdef DEBUGLIB
			printf("%d bytes available\n", bytesAvailable);
#endif
			if(bytesAvailable == 0)
				continue;
			if(bytesAvailable > 255)
				bytesAvailable = 255;
			unsigned char * buffer = (unsigned char *)malloc(
				sizeof(unsigned char) * (bytesSaved + bytesAvailable + 1));
			int bytesRead = CommPort_readEx(lib->comm, buffer + bytesSaved, bytesAvailable, lib->requestAliveInterval);
			bytesAvailable = bytesRead < bytesAvailable?bytesRead:bytesAvailable;
			if(bytesAvailable > 0)
			{
#ifdef DEBUGLIB
				printf("%d bytes read\n", bytesAvailable);
				printf("%d bytes saved\n", bytesSaved);
#endif
				buffer[bytesSaved + bytesAvailable] = 0;
				memcpy(buffer, savedBuff, bytesSaved);
				int r = _PesoLib_dataReceived(lib, buffer, bytesSaved + bytesAvailable, 
					lib->deviceIndex, 0);
				bytesSaved = bytesSaved + bytesAvailable;
				if(r == 0 && bytesSaved < sz)
				{
					free(savedBuff);
					savedBuff = buffer;
					continue;
				}
				bytesSaved = 0;
				if(r != 0) 
				{					
					free(buffer);
					continue;
				}
			}
			free(buffer);
		}
		if(lib->canceled == 1)
			continue;
		else
		{
			if(!_PesoLib_echoTest(lib, lib->comm, 1))
			{
				if(lib->canceled == 1)
					continue;
				_PesoLib_reconnect(lib);// try connect again
			} 
			else 
			{
#ifdef DEBUGLIB
				printf("device alive\n");
#endif
			}
		}
	}
	Mutex_unlock(lib->mutex);
	free(savedBuff);
#ifdef DEBUGLIB
	printf("leave _PesoLib_receiveFunc\n");
#endif
}

static int _PesoLib_echoTest(PesoLib * lib, CommPort* comm, int testCount)
{
#ifdef DEBUGLIB
	printf("Echo test started\n");
#endif
	int count = DeviceManager_getDeviceCount(gDevMan);
	int i, size, oldSize, bytesAvailable, bwritten, curr_index;
	unsigned char buffer[16], oldBuffer[16];
	size = 0;
	int dev_index = lib->deviceIndex;
	for(i = 0; i < count; i++)
	{
		curr_index = i;
		if(i == 0 && dev_index >= 0)
			curr_index = dev_index;
		else if(i == dev_index && dev_index >= 0)
			curr_index = 0;
		Device * dev = DeviceManager_getDevice(gDevMan, curr_index);
		oldSize = size;
		memcpy(oldBuffer, buffer, size);
		size = Device_makeCmd(dev, "getweight", NULL, buffer, 16);
		const char * csz = Device_getProperty(dev, "response_size");
		int sz = (csz == NULL? 0: atoi(csz));
		// check for errors or alreay sent command
		if(size <= 0 || (size == oldSize && memcmp(buffer, oldBuffer, size) == 0))
			continue;
#ifdef DEBUGLIB
		printf("trying communication with %s device driver\n", Device_getProperty(dev, "name"));
#endif
		Mutex_lock(lib->writeMutex);
		bwritten = CommPort_writeEx(comm, buffer, size, lib->connectTimeout / 2);
		Mutex_unlock(lib->writeMutex);
		if(bwritten != size)
			break;
#ifdef DEBUGLIB
		printf("%d byte written\n", bwritten);
#endif
		int savedBytes = 0;
		unsigned char * savedBuff = (unsigned char *)malloc(sizeof(unsigned char));
		while(1)
		{
			if(!CommPort_waitEx(comm, &bytesAvailable, lib->connectTimeout / 2))
			{
				free(savedBuff);
				break;
			}
#ifdef DEBUGLIB
			printf("%d bytes available\n", bytesAvailable);
#endif
			if(bytesAvailable <= 0)
			{
				free(savedBuff);
				break;
			}
			if(bytesAvailable > 255)
				bytesAvailable = 255;
			unsigned char * buff = (unsigned char *)malloc(
					sizeof(unsigned char) * (savedBytes + bytesAvailable + 1));
			int bytesRead = CommPort_readEx(comm, buff + savedBytes, bytesAvailable, lib->connectTimeout / 2);
			bytesAvailable = bytesRead < bytesAvailable?bytesRead:bytesAvailable;
			if(bytesAvailable <= 0)
			{
				free(buff);
				free(savedBuff);
				break;
			}
#ifdef DEBUGLIB
			printf("%d bytes read\n", bytesAvailable);
			printf("%d bytes available\n", savedBytes + bytesAvailable);
#endif
			buff[savedBytes + bytesAvailable] = 0;
			memcpy(buff, savedBuff, savedBytes);
			free(savedBuff);
			if(_PesoLib_dataReceived(lib, buff, savedBytes + bytesAvailable, curr_index, testCount))
			{
				free(buff);
				lib->deviceIndex = curr_index;
				return 1;
			}
			if(savedBytes >= sz)
			{
				free(buff);
				break;
			}
			savedBuff = buff;
			savedBytes = savedBytes + bytesAvailable;
		}
	}
#ifdef DEBUGLIB
	printf("No compatible device driver\n");
#endif
	return 0;
}

static void _PesoLib_connectFunc(void* data)
{
	PesoLib * lib = (PesoLib*)data;
	CommPort* comm;
	int i, need, count, len, tried;
	char * ports, * port;
#ifdef DEBUGLIB
	printf("enter _PesoLib_connectFunc\n");
#endif
	Mutex_lock(lib->mutex);
	lib->canceled = 0;
	Mutex_unlock(lib->mutex);
	count = 0;
	need = CommPort_enum(NULL, 0);
	if(lib->port[0] != 0)
		len = strlen(lib->port) + 1;
	else
		len = 0;
	ports = (char*)malloc(need + len + 1);
	if(len > 0)
	{
		memcpy(ports, lib->port, len);
		ports[len] = 0;
		count++;
	}
	count += CommPort_enum(ports + len, need);
#ifdef DEBUGLIB
	printf("searching port... %d found\n", count);
#endif
	tried = 1;
	while(lib->canceled == 0)
	{
		// try connect to one port
		comm = NULL;
		port = ports;
		for(i = 1; i <= count; i++)
		{
#ifdef DEBUGLIB
			printf("trying connect to %s baund %d\n", port, lib->commSettings.baund);
#endif
			comm = CommPort_createEx(port, &lib->commSettings);
			if(comm != NULL)
			{
				if(_PesoLib_echoTest(lib, comm, 0x7FFFFFFF))
					break;
				CommPort_free(comm);
				comm = NULL;
			}
			if(lib->canceled == 1)
				break;
			port += strlen(port) + 1;
		}
		if(comm != NULL)
		{
#ifdef DEBUGLIB
			printf("%s connected\n", port);
#endif
			// connection successful, start receive event
			lib->comm = comm;
			strcpy(lib->port, port);
			Event_post(lib->evConnect);
			Thread_start(lib->thReceive);
			break;
		}
#ifdef DEBUGLIB
		printf("no port available, trying again\n");
#endif
		if(lib->canceled == 1)
			break;
		// not port available, wait few seconds and try again
		Thread_wait(lib->retryTimeout);
		lib->baundIndex = (lib->baundIndex + 1) % (sizeof(baundValues) / sizeof(int));
		if(tried < 3 && baundValues[lib->baundIndex] > 9600)
		{
			lib->baundIndex = 0;
			tried++;
		}
		else if(tried >= 3 && lib->baundIndex == 0)
		{
			tried = 1;
		}
		lib->commSettings.baund = baundValues[lib->baundIndex];
	}
	free(ports);
#ifdef DEBUGLIB
	printf("leave _PesoLib_connectFunc\n");
#endif
}

static int _PesoLib_getProperty(const char * lwconfig, const char * config, 
	const char * key, 
	char * buffer, int size)
{
	char * pos = strstr(lwconfig, key);
	if(pos == NULL)
		return 0;
	pos = strstr(pos, ":");
	if(pos == NULL)
		return 0;
	pos++;
	char * end = strstr(pos, ";");
	int count = strlen(pos);
	if(end != NULL)
		count = end - pos;
	if(size < count)
		return count - size;
	strncpy(buffer, config + (pos - lwconfig), count);
	buffer[count] = 0;
#ifdef DEBUGLIB
	printf("property %s: value %s\n", key, buffer);
#endif
	return 1;
}

LIBEXPORT PesoLib * LIBCALL PesoLib_cria(const char* config)
{
	PesoLib * lib = (PesoLib*)malloc(sizeof(PesoLib));
	lib->baundIndex = 0;
	lib->mutex = Mutex_create();
	lib->writeMutex = Mutex_create();
	lib->price = 0.0f;
	lib->weight = 0;
	lib->stable = 0;
	lib->canceled = 0;
	lib->evCancel = Event_create();
	lib->evConnect = Event_createEx(0, 0);
	lib->evReceive = Event_createEx(0, 0);
	lib->thReceive = Thread_create(_PesoLib_receiveFunc, lib);
	lib->thConnect = Thread_create(_PesoLib_connectFunc, lib);
	lib->comm = NULL;
	lib->port[0] = 0;
	lib->commSettings.baund = baundValues[lib->baundIndex];
	lib->commSettings.data = 8;
	lib->commSettings.stop = StopBits_One;
	lib->commSettings.parity = Parity_None;
	lib->commSettings.flow = Flow_None;
	lib->deviceIndex = -1;
	lib->connectTimeout = 1000;
	lib->retryTimeout = 1500;
	lib->requestAliveInterval = 10000;
	lib->config = StringBuilder_create();
	PesoLib_setConfiguracao(lib, config);
	Thread_start(lib->thConnect);
	return lib;
}

LIBEXPORT int LIBCALL PesoLib_isConectado(PesoLib * lib)
{
	return lib->comm != NULL;
}

LIBEXPORT void LIBCALL PesoLib_setConfiguracao(PesoLib * lib, const char * config)
{
	if(config == NULL)
		return;
	char buff[PROPERY_BUFFER_SIZE];
	char * lwconfig = (char*)malloc(sizeof(char) * (strlen(config) + 1));
	strcpy(lwconfig, config);
	strlwr(lwconfig);
	int portChanged = 0;
	int commSettingsChanged = 0;
	if(_PesoLib_getProperty(lwconfig, config, "port", buff, PROPERY_BUFFER_SIZE))
	{
		if(strcmp(lib->port, buff) != 0)
			portChanged = 1;
		strcpy(lib->port, buff);
	}
	if(_PesoLib_getProperty(lwconfig, config, "baund", buff, PROPERY_BUFFER_SIZE))
	{
		lib->commSettings.baund = atoi(buff);
		commSettingsChanged = 1;
	}
	if(_PesoLib_getProperty(lwconfig, config, "data", buff, PROPERY_BUFFER_SIZE))
	{
		lib->commSettings.data = atoi(buff);
		commSettingsChanged = 1;
	}
	if(_PesoLib_getProperty(lwconfig, config, "stop", buff, PROPERY_BUFFER_SIZE))
	{
		if(strcmp(buff, "1.5") == 0)
			lib->commSettings.stop = StopBits_OneAndHalf;
		else if(strcmp(buff, "2") == 0)
			lib->commSettings.stop = StopBits_Two;
		else
			lib->commSettings.stop = StopBits_One;
		commSettingsChanged = 1;
	}
	if(_PesoLib_getProperty(lwconfig, config, "parity", buff, PROPERY_BUFFER_SIZE))
	{
		if(strcmp(buff, "space") == 0)
			lib->commSettings.parity = Parity_Space;
		else if(strcmp(buff, "mark") == 0)
			lib->commSettings.parity = Parity_Mark;
		else if(strcmp(buff, "even") == 0)
			lib->commSettings.parity = Parity_Even;
		else if(strcmp(buff, "odd") == 0)
			lib->commSettings.parity = Parity_Odd;
		else
			lib->commSettings.parity = Parity_None;
		commSettingsChanged = 1;
	}
	if(_PesoLib_getProperty(lwconfig, config, "flow", buff, PROPERY_BUFFER_SIZE))
	{
		if(strcmp(buff, "dsrdtr") == 0)
			lib->commSettings.flow = Flow_DSRDTR;
		else if(strcmp(buff, "rtscts") == 0)
			lib->commSettings.flow = Flow_RTSCTS;
		else if(strcmp(buff, "xonxoff") == 0)
			lib->commSettings.flow = Flow_XONXOFF;
		else
			lib->commSettings.flow = Flow_None;
		commSettingsChanged = 1;
	}
	if(_PesoLib_getProperty(lwconfig, config, "timeout", buff, PROPERY_BUFFER_SIZE))
	{
		int tm = atoi(buff);
		if(tm >= 50)
			lib->connectTimeout = tm;
	}
	if(_PesoLib_getProperty(lwconfig, config, "retry", buff, PROPERY_BUFFER_SIZE))
	{
		int tm = atoi(buff);
		if(tm >= 0)
			lib->retryTimeout = tm;
	}
	if(_PesoLib_getProperty(lwconfig, config, "alive", buff, PROPERY_BUFFER_SIZE))
	{
		int tm = atoi(buff);
		if(tm >= 1000)
			lib->requestAliveInterval = tm;
	}
	free(lwconfig);
	if(PesoLib_isConectado(lib))
	{
		if(portChanged)
		{
			lib->canceled = 1;
			CommPort_cancel(lib->comm);
			Thread_join(lib->thReceive);
			_PesoLib_reconnect(lib);// try connect again
		} 
		else if(commSettingsChanged) 
		{
			if(!CommPort_configure(lib->comm, &lib->commSettings))
			{
				lib->canceled = 1;
				CommPort_cancel(lib->comm);
				Thread_join(lib->thReceive);
				_PesoLib_reconnect(lib);// try connect again
			}
		}
	}
}

LIBEXPORT const char* LIBCALL PesoLib_getConfiguracao(PesoLib * lib)
{
	StringBuilder_clear(lib->config);
	// Port
	if(lib->port[0] != 0)
		StringBuilder_appendFormat(lib->config, "port:%s;", lib->port);
	// Baund
	StringBuilder_appendFormat(lib->config, "baund:%d;", lib->commSettings.baund);
	// Data Bits
	StringBuilder_appendFormat(lib->config, "data:%d;", (int)lib->commSettings.data);
	// Stop Bits
	StringBuilder_append(lib->config, "stop:");
	if(lib->commSettings.stop == StopBits_OneAndHalf)
		StringBuilder_append(lib->config, "1.5;");
	else if(lib->commSettings.stop == StopBits_Two)
		StringBuilder_append(lib->config, "2;");
	else
		StringBuilder_append(lib->config, "1;");
	// Parity
	StringBuilder_append(lib->config, "parity:");
	if(lib->commSettings.parity == Parity_Space)
		StringBuilder_append(lib->config, "space;");
	else if(lib->commSettings.parity == Parity_Mark)
		StringBuilder_append(lib->config, "mark;");
	else if(lib->commSettings.parity == Parity_Even)
		StringBuilder_append(lib->config, "even;");
	else if(lib->commSettings.parity == Parity_Odd)
		StringBuilder_append(lib->config, "odd;");
	else
		StringBuilder_append(lib->config, "none;");
	// Flow
	StringBuilder_append(lib->config, "flow:");
	if(lib->commSettings.flow == Flow_DSRDTR)
		StringBuilder_append(lib->config, "dsrdtr;");
	else if(lib->commSettings.flow == Flow_RTSCTS)
		StringBuilder_append(lib->config, "rtscts;");
	else if(lib->commSettings.flow == Flow_XONXOFF)
		StringBuilder_append(lib->config, "xonxoff;");
	else
		StringBuilder_append(lib->config, "none;");
	// timeout
	StringBuilder_appendFormat(lib->config, "timeout:%d;", lib->connectTimeout);
	StringBuilder_appendFormat(lib->config, "retry:%d;", lib->retryTimeout);
	StringBuilder_appendFormat(lib->config, "alive:%d;", lib->requestAliveInterval);
	return StringBuilder_getData(lib->config);
}

LIBEXPORT const char* LIBCALL PesoLib_getMarcas(PesoLib * lib)
{
	return DeviceManager_getNames(gDevMan);
}

LIBEXPORT const char* LIBCALL PesoLib_getModelos(PesoLib * lib, const char* marca)
{
	return DeviceManager_getModels(gDevMan, marca);
}

LIBEXPORT int LIBCALL PesoLib_aguardaEvento(PesoLib * lib)
{
	Event* events[3];
#ifdef DEBUGLIB
		printf("PesoLib_aguardaEvento\n");
#endif
	events[0] = lib->evCancel;
	events[1] = lib->evConnect;
	events[2] = lib->evReceive;
	Event* object = Event_waitMultiple(events, 3);
	if(object == lib->evCancel)
		return Evento_Cancelado;
	if(object == lib->evConnect)
	{
		if(PesoLib_isConectado(lib))
			return Evento_Conectado;
		return Evento_Desconectado;
	}
	return Evento_PesoRecebido;
}

LIBEXPORT int LIBCALL PesoLib_getUltimoPeso(PesoLib * lib)
{
	return lib->weight;
}

LIBEXPORT int LIBCALL PesoLib_recebePeso(PesoLib * lib, int* gramas)
{
	Event* events[2];
	
	events[0] = lib->evCancel;
	events[1] = lib->evReceive;
	if(Event_waitMultiple(events, 2) != lib->evReceive)
		return 0;
	*gramas = lib->weight;
	return 1;
}

LIBEXPORT int LIBCALL PesoLib_solicitaPeso(PesoLib * lib, float preco)
{
	if(!PesoLib_isConectado(lib) || lib->deviceIndex < 0)
		return 0;
	unsigned char buffer[16];
	char bufPreco[16];
	int size;
	sprintf(bufPreco, "%d", (int)(preco * 100));
	Device * dev = DeviceManager_getDevice(gDevMan, lib->deviceIndex);
	size = Device_makeCmd(dev, "setprice", bufPreco, buffer, 16);
	if(size > 0 && preco >= 0.01)
	{
		Mutex_lock(lib->writeMutex);
		CommPort_write(lib->comm, buffer, size);
		Mutex_unlock(lib->writeMutex);
	}
	size = Device_makeCmd(dev, "getweight", bufPreco, buffer, 16);
	if(size <= 0)
		return 0;
	Mutex_lock(lib->writeMutex);
	int bwritten = CommPort_write(lib->comm, buffer, size);
	Mutex_unlock(lib->writeMutex);
	if(bwritten != size)
		return 0;
	return 1;
}

LIBEXPORT void LIBCALL PesoLib_cancela(PesoLib * lib)
{
	lib->canceled = 1;
	if(lib->comm != NULL)
		CommPort_cancel(lib->comm);
	Event_post(lib->evCancel);
}

LIBEXPORT void LIBCALL PesoLib_libera(PesoLib * lib)
{
	PesoLib_cancela(lib);
	Event_free(lib->evCancel);
	Event_free(lib->evReceive);
	Event_free(lib->evConnect);
	Thread_join(lib->thReceive);
	Thread_free(lib->thReceive);
	Thread_join(lib->thConnect);
	Thread_free(lib->thConnect);
	if(lib->comm != NULL)
		CommPort_free(lib->comm);
	StringBuilder_free(lib->config);
	Mutex_free(lib->writeMutex);
	Mutex_free(lib->mutex);
	free(lib);
}

LIBEXPORT const char* LIBCALL PesoLib_getVersao(PesoLib * lib)
{
	return gVersion;
}

int PesoLib_inicializa()
{
	if(gDevMan != NULL)
		return 1;
#ifdef DEBUGLIB
	printf("testing string builder\n");
	StringBuilder* builder = StringBuilder_create();
	StringBuilder_append(builder, "Olá Mundo!");
	StringBuilder_append(builder, "\n");
	StringBuilder_getData(builder);
	int i;
	for(i = 0; i < 10000; i++)
		StringBuilder_append(builder, "Composes a string with the same text that would be printed if format was used on printf, but using the elements in the variable argument list identified by arg instead of additional function arguments and storing the resulting content as a C string in the buffer pointed by s (taking n as the maximum buffer capacity to fill).");
	StringBuilder_getData(builder);
	StringBuilder_append(builder, "\n");
	StringBuilder_clear(builder);
	StringBuilder_append(builder, "Composes a string with the same text that would be printed if format was used on printf, but using the elements in the variable argument list identified by arg instead of additional function arguments and storing the resulting content as a C string in the buffer pointed by s (taking n as the maximum buffer capacity to fill).");
	StringBuilder_append(builder, "\n");
	StringBuilder_getData(builder);
	for(i = 0; i < 10000; i++)
		StringBuilder_appendFormat(builder, "%d %.2f %s %c %p\n", 256, 1.5f, "String", 'C', builder);
	StringBuilder_getData(builder);
	StringBuilder_free(builder);
	printf("library initialized\n");
#endif
	gDevMan = DeviceManager_create();
	return 1;
}

void PesoLib_finaliza()
{
	if(gDevMan == NULL)
		return;
	DeviceManager_free(gDevMan);
}
