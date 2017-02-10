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
#define BUFFER_SIZE 256
#define CMD_SIZE 16

#ifndef DEBUGLIB
//#define DEBUGLIB
#endif

static DeviceManager * gDevMan = NULL;
static const char gVersion[] = "1.2.0.0";
static const int REPEAT_BAUND = 19200;
static const int baundValues[] = {9600, 2400, 4800, 19200, 14400, 38400, 57600, 115200, 230400, 460800, 921600, 110, 300, 600, 1200};

struct PesoLib
{
	int canceled;
	int connected;
	int abandoned;
	int testing;
	Thread* thConnect;
	Thread* thReceive;
	Thread* thAlive;
	Event* evCancel;
	Event* evDetected;
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
	Mutex* writeMutex;
};

static int _PesoLib_echoTest(PesoLib * lib);

static void _PesoLib_disconnect(PesoLib * lib)
{
#ifdef DEBUGLIB
	printf("Disconnecting device\n");
#endif
	lib->abandoned = 1;
	Mutex_lock(lib->writeMutex);
	if(lib->comm != NULL)
		CommPort_cancel(lib->comm);
	Mutex_unlock(lib->writeMutex);
	Thread_join(lib->thReceive);
	Mutex_lock(lib->writeMutex);
	if(lib->comm != NULL)
		CommPort_free(lib->comm);
	lib->comm = NULL;
	Mutex_unlock(lib->writeMutex);
	lib->abandoned = 0;
	lib->connected = 0;
#ifdef DEBUGLIB
	printf("Device disconnected\n");
#endif
}

static void _PesoLib_cancel(PesoLib * lib)
{
#ifdef DEBUGLIB
	printf("Canceling connection\n");
#endif
	_PesoLib_disconnect(lib);
	if(lib->canceled)
		Event_post(lib->evCancel);
	Event_post(lib->evConnect);
#ifdef DEBUGLIB
	printf("Connection canceled\n");
#endif
}

static void _PesoLib_reconnect(PesoLib * lib)
{
	_PesoLib_cancel(lib);
	Thread_start(lib->thConnect); // try connect again
}

static int _PesoLib_dataReceived(PesoLib * lib, const unsigned char * buffer, 
	int size)
{
	int count = DeviceManager_getDeviceCount(gDevMan);
	int dev_index = lib->deviceIndex;
	int max_response_size = 0;
	int consummed = 0;
	int min_response, max_response, i, index, offset;
#ifdef DEBUGLIB
	printf("Data received (%d byte): %s\n", size, buffer);
#endif
	for(i = 0; i < count; i++)
	{
		index = i;
		if(i == 0 && dev_index >= 0)
			index = dev_index;
		else if(i == dev_index && dev_index >= 0)
			index = 0;
		Device * dev = DeviceManager_getDevice(gDevMan, index);
		Device_getResponseRange(dev, &min_response, &max_response);
		max_response_size = max_response > max_response_size?max_response:max_response_size;
#ifdef DEBUGLIB
		printf("Using %s driver\n", Device_getName(dev));
#endif
		offset = 0;
		while((size - offset) >= min_response)
		{
			consummed = Device_execute(dev, buffer + offset, size - offset);
			if(consummed)
				break;
			offset++;
		}
		if(!consummed)
			continue;
		consummed += offset;
		Event_post(lib->evDetected);
		dev_index = index;
		if (lib->testing)
			break;
		Event_reset(lib->evDetected);
#ifdef DEBUGLIB
		printf("Checking stable %s = %d\n", Device_getName(dev), Device_isStable(dev));
#endif
		if(!Device_isStable(dev))
			break;
		lib->weight = Device_getWeight(dev);
		Event_post(lib->evReceive);
		break;
	}
	lib->deviceIndex = dev_index; // novo dispositivo ou restaura anterior
	if(consummed == 0 && size > max_response_size)
		consummed = size - max_response_size + 1; // já verificou esses bytes
	return consummed;
}

static void _PesoLib_aliveTest(void* data)
{
	PesoLib * lib = (PesoLib*)data;
	if(!_PesoLib_echoTest(lib))
	{
		if (lib->canceled)
			return;
		_PesoLib_reconnect(lib);// try connect again
	}
	else 
	{
#ifdef DEBUGLIB
		printf("Device alive\n");
#endif
	}
}

static void _PesoLib_receiveFunc(void* data)
{
	PesoLib * lib = (PesoLib*)data;
	unsigned char buffer[BUFFER_SIZE];
	unsigned char window[BUFFER_SIZE + 1];
	int window_size = 0;
	int buffer_size;
	int remaining, consumed, unused, used;

	while(!lib->canceled && !lib->abandoned)
	{
#ifdef DEBUGLIB
	printf("Waiting for receive event\n");
#endif
		buffer_size = CommPort_readEx(lib->comm, buffer, BUFFER_SIZE, lib->requestAliveInterval);
#ifdef DEBUGLIB
		printf("Bytes received: %d\n", buffer_size);
#endif
		buffer_size = buffer_size < BUFFER_SIZE?buffer_size:BUFFER_SIZE;
		if(lib->canceled || lib->abandoned)
			continue;
		if(buffer_size == 0 && !lib->testing)
		{
			Thread_start(lib->thAlive);
			continue;
		}
		if(buffer_size == 0)
			continue;
#ifdef DEBUGLIB
		printf("Bytes read: %d\n", buffer_size);
#endif
		remaining = buffer_size;
		consumed = 0;
		while(remaining > 0 || consumed > 0)
		{
			unused = BUFFER_SIZE - window_size;
			consumed = remaining < unused?remaining:unused;
			if(consumed == 0 && remaining > 0)
				consumed++; // consome pelo menos 1 byte
			used = buffer_size - remaining;
			remaining -= consumed;
			if(consumed > unused)
			{
				memmove(window, window + consumed, BUFFER_SIZE - consumed);
				window_size -= consumed;
			}
			memcpy(window + window_size, buffer + used, consumed);
			window_size += consumed;
#ifdef DEBUGLIB
			window[window_size] = 0; // permite mostrar como texto
#endif
			consumed = _PesoLib_dataReceived(lib, window, window_size);
			window_size -= consumed;
			if(window_size > 0)
				memmove(window, window + consumed, BUFFER_SIZE - consumed);
			if(consumed > 0 && window_size == 0)
				consumed = 0;
		}
	}
}

static int _PesoLib_echoTest(PesoLib * lib)
{
	int count = DeviceManager_getDeviceCount(gDevMan);
	int cmds_count; // unique cmd count
	unsigned char ** cmds = (unsigned char**)malloc((sizeof(unsigned char*) + CMD_SIZE) * count);
	int * cmds_size = (int*)malloc(sizeof(int) * count);
	
	int i, j, size, index, result = 0;
	unsigned char buffer[CMD_SIZE];
	int dev_index = lib->deviceIndex;
	int old_testing = lib->testing;

#ifdef DEBUGLIB
	printf("Echo test started\n");
#endif
	cmds_count = 0;	
	lib->testing = 1;
	for(i = 0; i < count; i++)
	{
		if(lib->canceled || lib->abandoned)
			break;
		index = i;
		if(i == 0 && dev_index >= 0)
			index = dev_index;
		else if(i == dev_index && dev_index >= 0)
			index = 0;
		cmds[i] = (unsigned char *)(cmds + count) + i * CMD_SIZE;
		Device * dev = DeviceManager_getDevice(gDevMan, index);
		size = Device_makeCmd(dev, DEV_CMD_GET_WEIGHT, NULL, buffer, CMD_SIZE);
		// procura comando repedido
		for(j = 0; j < cmds_count; j++)
		{
			if(cmds_size[j] == size && memcmp(cmds[j], buffer, size) == 0)
				break;
		}
		if(j < cmds_count)
			continue; // comando repetido
		memcpy(cmds[cmds_count], buffer, size);
		cmds_size[cmds_count] = size;
		cmds_count++;
		// código único e primeiro dispositivo, tenta comunicação
		lib->deviceIndex = index;
		PesoLib_solicitaPeso(lib, 0.00);
		Event* object = Event_waitMultipleEx(&lib->evDetected, 1, lib->connectTimeout);
		if(object != lib->evDetected)
			continue;
		dev_index = lib->deviceIndex;
		result = 1;
		break;
	}
	free(cmds_size);
	free(cmds);
	lib->testing = old_testing;
	lib->deviceIndex = dev_index; // novo dispositivo ou restaura anterior
	return result;
}

static void _PesoLib_connectFunc(void* data)
{
	PesoLib * lib = (PesoLib*)data;
	CommPort* comm;
	int i, need, count, len, tried;
	char * ports, * port;
#ifdef DEBUGLIB
	printf("Starting connection\n");
#endif
	if(lib->canceled || lib->abandoned) 
	{
#ifdef DEBUGLIB
	printf("Connection aborted\n");
#endif
		return;
	}
	i = 0;
	count = 0;
	need = 4096;
	do
	{
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
		int port_count = CommPort_enum(ports + len, need);
		if(port_count == 0)
		{
#ifdef DEBUGLIB
	printf("Error on enum ports\n");
#endif
			break;
		}
		if(port_count < 0)
		{
			if(i > 1)
			{
#ifdef DEBUGLIB
		printf("Error trying to enum ports again, stoped!\n");
#endif
				break;
			}
#ifdef DEBUGLIB
	printf("Enum ports, need more %d byte of memory\n", -port_count);
#endif
			need = -port_count;
			i++;
			free(ports);
			continue;	
		}
		count += port_count;
		break;
	} while(1);
#ifdef DEBUGLIB
	printf("Searching port... %d found\n", count);
#endif
	tried = 1;
	while(!lib->canceled && !lib->abandoned)
	{
		// try connect to one port
		comm = NULL;
		port = ports;
		for(i = 1; i <= count; i++)
		{
#ifdef DEBUGLIB
			printf("Trying connect to %s baund %d\n", port, lib->commSettings.baund);
#endif
			comm = CommPort_createEx(port, &lib->commSettings);
			if(comm != NULL)
			{
				// connection successful, start receive event
				lib->comm = comm;
				Thread_start(lib->thReceive);
				if(_PesoLib_echoTest(lib))
					break;
				_PesoLib_disconnect(lib);
				comm = NULL;
			}
			if(lib->canceled || lib->abandoned)
				break;
			port += strlen(port) + 1;
		}
		if(comm != NULL)
		{
#ifdef DEBUGLIB
			Device * dev = DeviceManager_getDevice(gDevMan, lib->deviceIndex);
			printf("Device %s connected using %s\n", Device_getName(dev), port);
#endif
			lib->connected = 1;
			strcpy(lib->port, port);
			Event_post(lib->evConnect);
			break;
		}
#ifdef DEBUGLIB
		printf("No port available, trying again\n");
#endif
		if(lib->canceled || lib->abandoned)
			break;
		// not port available, wait few seconds and try again
		Thread_wait(lib->retryTimeout);
		lib->baundIndex = (lib->baundIndex + 1) % (sizeof(baundValues) / sizeof(int));
		if(tried < 3 && baundValues[lib->baundIndex] > REPEAT_BAUND)
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
	printf("Property %s: value %s\n", key, buffer);
#endif
	return 1;
}

LIBEXPORT PesoLib * LIBCALL PesoLib_cria(const char* config)
{
	PesoLib * lib = (PesoLib*)malloc(sizeof(PesoLib));
	lib->connected = 0;
	lib->testing = 0;
	lib->abandoned = 0;
	lib->canceled = 0;
	lib->baundIndex = 0;
	lib->writeMutex = Mutex_create();
	lib->price = 0.0f;
	lib->weight = 0;
	lib->stable = 0;
	lib->evCancel = Event_create();
	lib->evConnect = Event_createEx(0, 0);
	lib->evDetected = Event_createEx(0, 0);
	lib->evReceive = Event_createEx(0, 0);
	lib->thReceive = Thread_create(_PesoLib_receiveFunc, lib);
	lib->thConnect = Thread_create(_PesoLib_connectFunc, lib);
	lib->thAlive = Thread_create(_PesoLib_aliveTest, lib);
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
	return lib->connected;
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
	if(lib->comm == NULL)
		return;
	if(!portChanged && !commSettingsChanged)
		return;
	if(commSettingsChanged) 
		CommPort_configure(lib->comm, &lib->commSettings);
	_PesoLib_reconnect(lib);
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
	printf("Waiting weight event\n");
#endif
	events[0] = lib->evCancel;
	events[1] = lib->evConnect;
	events[2] = lib->evReceive;
	Event* object = Event_waitMultiple(events, 3);
	if(object == events[0])
	{
#ifdef DEBUGLIB
		printf("Event cancelled triggered\n");
#endif
		return Evento_Cancelado;
	}
	if(object == lib->evConnect)
	{
#ifdef DEBUGLIB
		printf("Event connect triggered: IsConnected: %d\n", PesoLib_isConectado(lib));
#endif
		if(PesoLib_isConectado(lib))
			return Evento_Conectado;
		return Evento_Desconectado;
	}
#ifdef DEBUGLIB
		printf("Event weight received\n");
#endif
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
	if(Event_waitMultiple(events, 2) != events[1])
		return 0;
	*gramas = lib->weight;
	return 1;
}

LIBEXPORT int LIBCALL PesoLib_solicitaPeso(PesoLib * lib, float preco)
{
	unsigned char buffer[CMD_SIZE];
	char bufPreco[20];
	int size;
	int price_size;
	int bwritten = 0;
	
	if(lib->comm == NULL || lib->deviceIndex < 0)
		return 0;
#ifdef DEBUGLIB
	printf("Requesting weight\n");
#endif
	sprintf(bufPreco, "%d", (int)(preco * 100));
	Device * dev = DeviceManager_getDevice(gDevMan, lib->deviceIndex);
	price_size = Device_makeCmd(dev, DEV_CMD_SET_PRICE, bufPreco, buffer, CMD_SIZE);
	if(price_size > 0 && preco > 0.005)
	{
		Mutex_lock(lib->writeMutex);
		bwritten = CommPort_writeEx(lib->comm, buffer, price_size, lib->connectTimeout / 2);
		Mutex_unlock(lib->writeMutex);
	}
	size = Device_makeCmd(dev, DEV_CMD_GET_WEIGHT, bufPreco, buffer, CMD_SIZE);
	if(size == 0)
		return bwritten == price_size;
	Mutex_lock(lib->writeMutex);
	bwritten = CommPort_writeEx(lib->comm, buffer, size, lib->connectTimeout / 2);
	Mutex_unlock(lib->writeMutex);
	return bwritten == size;
}

LIBEXPORT void LIBCALL PesoLib_cancela(PesoLib * lib)
{
	if(lib->canceled)
		return;
	lib->canceled = 1;
	_PesoLib_cancel(lib);
}

LIBEXPORT void LIBCALL PesoLib_libera(PesoLib * lib)
{
	PesoLib_cancela(lib);
	Thread_join(lib->thAlive);
	Thread_join(lib->thReceive);
	Thread_join(lib->thConnect);
	Thread_free(lib->thAlive);
	Thread_free(lib->thReceive);
	Thread_free(lib->thConnect);
	Event_free(lib->evCancel);
	Event_free(lib->evConnect);
	Event_free(lib->evReceive);
	Event_free(lib->evDetected);
	StringBuilder_free(lib->config);
	Mutex_free(lib->writeMutex);
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
	gDevMan = DeviceManager_create();
	return 1;
}

void PesoLib_finaliza()
{
	if(gDevMan == NULL)
		return;
	DeviceManager_free(gDevMan);
}
