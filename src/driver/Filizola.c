#include "Filizola.h"
#include "DevicePrivate.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct Filizola
{
	int stable;
	int weight;
	char models[48];
	char buffer[16];
} Filizola;

int Filizola_init(Device* _dev)
{
	Filizola* dev = (Filizola*)_dev->data;
	dev->stable = 0;
	dev->weight = 0;
	strcpy(dev->buffer, "Filizola");
	strcpy(dev->models, ""); // TODO: add models
	return 1;
}

static int _Filizola_execute(const unsigned char* buffer, int size,
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
		if(i == 1 && buffer[i] == ' ')
			continue;
		if(i == 1 && buffer[i] == '-')
		{
			*stable = 0;
			return 7;
		}
		if(buffer[i] < '0' || buffer[i] > '9')
			return 0;
		_peso += mult * (buffer[i] - '0');
		mult *= 10;
	}
	*peso = _peso;
	*stable = _peso > 0;
	return 7;
}

int Filizola_execute(Device* _dev, const unsigned char* buffer, int size)
{
	Filizola* dev = (Filizola*)_dev->data;
	return _Filizola_execute(buffer, size, &dev->weight, &dev->stable);
}

int Filizola_isStable(Device * _dev)
{
	Filizola* dev = (Filizola*)_dev->data;
	return dev->stable;
}

int Filizola_getWeight(Device * _dev)
{
	Filizola* dev = (Filizola*)_dev->data;
	return dev->weight;
}

void Filizola_getResponseRange(Device * _dev, int * min, int * max)
{
	*min = 7;
	*max = 7;
}

const char* Filizola_getName(Device * _dev)
{
	Filizola* dev = (Filizola*)_dev->data;
	return dev->buffer;
}

const char* Filizola_getProperty(Device* _dev, const char* key)
{
	Filizola* dev = (Filizola*)_dev->data;
	if(strcmp(DEV_PROP_MODELS, key) == 0)
	{
		return dev->models;
	}
	return NULL;
}

int Filizola_makeCmd(Device* _dev, const char* func, const char * data, 
	unsigned char* cmdOut, int bufLen)
{
	//Filizola* dev = (Filizola*)_dev;
	if(strcmp(DEV_CMD_GET_WEIGHT, func) == 0)
	{
		const unsigned char cmd[] = { 0x05 };
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

void Filizola_free(Device* _dev)
{
	Filizola* dev = (Filizola*)_dev->data;
	free(dev);
	Device_release(_dev);
}

Device * Device_createFilizola()
{
	Filizola* _dev = (Filizola*)malloc(sizeof(Filizola));
	Device* dev = Device_alloc(_dev);
	dev->init = Filizola_init;
	dev->execute = Filizola_execute;
	dev->isStable = Filizola_isStable;
	dev->getWeight = Filizola_getWeight;
	dev->getResponseRange = Filizola_getResponseRange;
	dev->getName = Filizola_getName;
	dev->getProperty = Filizola_getProperty;
	dev->makeCmd = Filizola_makeCmd;
	dev->free = Filizola_free;
	return dev;
}
