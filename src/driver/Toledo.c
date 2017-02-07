#include "Toledo.h"
#include "DevicePrivate.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct Toledo
{
	int stable;
	int weight;
	char models[48];
	char buffer[16];
} Toledo;

int Toledo_init(Device* _dev)
{
	Toledo* dev = (Toledo*)_dev->data;
	dev->stable = 0;
	dev->weight = 0;
	strcpy(dev->buffer, "Toledo");
	strcpy(dev->models, ""); // TODO: add models
	return 1;
}

static int _Toledo_execute_live(const unsigned char* buffer, int size,
	int * peso, int * stable)
{
	if(size < 8 || buffer[0] != 0x02 || buffer[7] != 13)
		return 0;
	int i;
	int _peso = 0, mult = 1;
	// STX 00.250 CR
	for(i = 6; i >= 1; i--)
	{
		if(i == 3 && buffer[i] == '.')
			continue;
		if(i == 1 && buffer[i] == '-')
		{
			*stable = 0;
			return 8;
		}
		if(buffer[i] < '0' || buffer[i] > '9')
			return 0;
		_peso += mult * (buffer[i] - '0');
		mult *= 10;
	}
	*peso = _peso;
	*stable = _peso > 0;
	return 8;
}

static int _Toledo_execute_normal(const unsigned char* buffer, int size,
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

int Toledo_execute(Device* _dev, const unsigned char* buffer, int size)
{
	Toledo* dev = (Toledo*)_dev->data;
	int r = _Toledo_execute_normal(buffer, size, &dev->weight, &dev->stable);
	if(r != 0)
		return r;
	return _Toledo_execute_live(buffer, size, &dev->weight, &dev->stable);
}

int Toledo_isStable(Device * _dev)
{
	Toledo* dev = (Toledo*)_dev->data;
	return dev->stable;
}

int Toledo_getWeight(Device * _dev)
{
	Toledo* dev = (Toledo*)_dev->data;
	return dev->weight;
}

void Toledo_getResponseRange(Device * _dev, int * min, int * max)
{
	*min = 7;
	*max = 8;
}

const char* Toledo_getName(Device * _dev)
{
	Toledo* dev = (Toledo*)_dev->data;
	return dev->buffer;
}

const char* Toledo_getProperty(Device* _dev, const char* key)
{
	Toledo* dev = (Toledo*)_dev->data;
	if(strcmp(DEV_PROP_MODELS, key) == 0)
	{
		return dev->models;
	}
	return NULL;
}

int Toledo_makeCmd(Device* _dev, const char* func, const char * data, 
	unsigned char* cmdOut, int bufLen)
{
	//Toledo* dev = (Toledo*)_dev;
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
		if(data == NULL)
			return 0;
		unsigned char cmd[] = { 0x01, '0', '0', '0', '0', '0', '0', 0x03 };
		int bWritten = bufLen;
		if(bWritten > 8)
			bWritten = 8;
		int j, i, len = strlen(data);
		j = 6;
		for(i = len - 1; i >= 0; i--)
		{
			// skip , and .
			if(data[i] == '.' || data[i] == ',')
				continue;
			// invalid char
			if(data[i] < '0' || data[i] > '9')
				return 0;
			// limit excceded
			if(j == 1)
			{
				if(data[i] != '0')
					return 0;
				continue; // skip left zeroes
			}
			cmd[j] = data[i];
			j--;
		}
		memcpy(cmdOut, cmd, bWritten);
		return bWritten;
	}
	return 0;
}

void Toledo_free(Device* _dev)
{
	Toledo* dev = (Toledo*)_dev->data;
	free(dev);
	Device_release(_dev);
}

Device * Device_createToledo()
{
	Toledo * _dev = (Toledo*)malloc(sizeof(Toledo));
	Device * dev = Device_alloc(_dev);
	dev->init = Toledo_init;
	dev->execute = Toledo_execute;
	dev->isStable = Toledo_isStable;
	dev->getWeight = Toledo_getWeight;
	dev->getResponseRange = Toledo_getResponseRange;
	dev->getName = Toledo_getName;
	dev->getProperty = Toledo_getProperty;
	dev->makeCmd = Toledo_makeCmd;
	dev->free = Toledo_free;
	return dev;
}
