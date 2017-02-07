#include "Magna.h"
#include "DevicePrivate.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct Magna
{
	int stable;
	int weight;
	char models[48];
	char buffer[16];
} Magna;

int Magna_init(Device* _dev)
{
	Magna* dev = (Magna*)_dev->data;
	dev->stable = 0;
	dev->weight = 0;
	strcpy(dev->buffer, "Magna");
	strcpy(dev->models, ""); // TODO: add models
	return 1;
}

static int _Magna_execute_kg(const unsigned char* buffer, int size,
	int * peso, int * stable)
{
	if(size < 17 || buffer[0] != 'P' || buffer[1] != 'E' || buffer[2] != 'S' || 
			buffer[3] != 'O' || buffer[13] != 'k' || buffer[14] != 'g')
		return 0;
	int i;
	int _peso = 0, mult = 1;
	// PESO   1,328 kg
	for(i = 11; i >= 6; i--)
	{
		if(i == 8 && buffer[i] == ',')
			continue;
		if(i == 6 && buffer[i] == ' ')
			continue;
		if(i == 6 && buffer[i] == '-')
		{
			*stable = 0;
			return 17;
		}
		if(buffer[i] < '0' || buffer[i] > '9')
			return 0;
		_peso += mult * (buffer[i] - '0');
		mult *= 10;
	}
	*peso = _peso;
	*stable = _peso > 0;
	return 17;
}

static int _Magna_execute_stx(const unsigned char* buffer, int size,
	int * peso, int * stable)
{
	if(size < 7 || buffer[0] != 0x02 || buffer[6] != 0x03)
		return 0;
	int i;
	// STX IIIII ETX | STX NNNNN ETX | STX SSSSS ETX
	if(buffer[1] == 'I' || buffer[1] == 'N' || buffer[1] == 'S')
	{
		for(i = 5; i >= 2; i--)
		{
			if(buffer[1] != buffer[i])
				return 0;
		}
		*stable = 0;
		return 7;
	}
	int _peso = 0, mult = 1;
	// STX 01234 ETX
	for(i = 5; i >= 1; i--)
	{
		if(buffer[i] < '0' || buffer[i] > '9')
			return 0;
		_peso += mult * (buffer[i] - '0');
		mult *= 10;
	}
	*peso = _peso;
	*stable = _peso > 0;
	return 7;
}

int Magna_execute(Device* _dev, const unsigned char* buffer, int size)
{
	Magna* dev = (Magna*)_dev->data;
	int r = _Magna_execute_kg(buffer, size, &dev->weight, &dev->stable);
	if(r != 0)
		return r;
	return _Magna_execute_stx(buffer, size, &dev->weight, &dev->stable);
}

int Magna_isStable(Device * _dev)
{
	Magna* dev = (Magna*)_dev->data;
	return dev->stable;
}

int Magna_getWeight(Device * _dev)
{
	Magna* dev = (Magna*)_dev->data;
	return dev->weight;
}

void Magna_getResponseRange(Device * _dev, int * min, int * max)
{
	*min = 7;
	*max = 17;
}

const char* Magna_getName(Device * _dev)
{
	Magna* dev = (Magna*)_dev->data;
	return dev->buffer;
}

const char* Magna_getProperty(Device* _dev, const char* key)
{
	Magna* dev = (Magna*)_dev->data;
	if(strcmp(DEV_PROP_MODELS, key) == 0)
	{
		return dev->models;
	}
	return NULL;
}

int Magna_makeCmd(Device* _dev, const char* func, const char * data, 
	unsigned char* cmdOut, int bufLen)
{
	//Magna* dev = (Magna*)_dev;
	if(strcmp(DEV_CMD_GET_WEIGHT, func) == 0)
	{
		const unsigned char cmd[] = { 'P' };
		int bWritten = bufLen;
		if(bWritten > 1)
			bWritten = 1;
		memcpy(cmdOut, cmd, bWritten);
		return bWritten;
	}
	if(strcmp(DEV_CMD_SET_PRICE, func) == 0)
	{
		return 0; // not supported
	}
	return 0;
}

void Magna_free(Device* _dev)
{
	Magna* dev = (Magna*)_dev->data;
	free(dev);
	Device_release(_dev);
}

Device * Device_createMagna()
{
	Magna* _dev = (Magna*)malloc(sizeof(Magna));
	Device* dev = Device_alloc(_dev);
	dev->init = Magna_init;
	dev->execute = Magna_execute;
	dev->isStable = Magna_isStable;
	dev->getWeight = Magna_getWeight;
	dev->getResponseRange = Magna_getResponseRange;
	dev->getName = Magna_getName;
	dev->getProperty = Magna_getProperty;
	dev->makeCmd = Magna_makeCmd;
	dev->free = Magna_free;
	return dev;
}
