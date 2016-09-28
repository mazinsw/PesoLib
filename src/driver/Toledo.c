#include "Toledo.h"
#include "DevicePrivate.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct Toledo
{
	int peso;
	char models[48];
	char buffer[16];
} Toledo;


int Toledo_init(Device* _dev)
{
	Toledo* dev = (Toledo*)_dev->data;
	dev->peso = 0;
	strcpy(dev->models, ""); // TODO: add models
	return 1;
}

static int _Toledo_execute(const unsigned char* buffer, int size,
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
	*stable = 1;
	return 7;
}

int Toledo_execute(Device* _dev, const unsigned char* buffer, int size)
{
	Toledo* dev = (Toledo*)_dev->data;
	int peso, stable = 0;
	int used = _Toledo_execute(buffer, size, &peso, &stable);
	if(used == 0 || stable != 1)
		return 0;
	dev->peso = peso;
	return used;
}

int Toledo_test(Device* _dev, const unsigned char* buffer, int size)
{
	int peso, stable;
	return _Toledo_execute(buffer, size, &peso, &stable);
}

const char* Toledo_getProperty(Device* _dev, const char* key)
{
	Toledo* dev = (Toledo*)_dev->data;
	if(strcmp("weight", key) == 0)
	{
		sprintf(dev->buffer, "%d", dev->peso);
		return dev->buffer;
	}
	if(strcmp("name", key) == 0)
	{
		strcpy(dev->buffer, "Toledo");
		return dev->buffer;
	}
	if(strcmp("models", key) == 0)
	{
		return dev->models;
	}
	if(strcmp("response_size", key) == 0)
	{
		static const char * rsize = "7";
		return rsize;
	}
	return NULL;
}

int Toledo_makeCmd(Device* _dev, const char* func, const char * data, 
	unsigned char* cmdOut, int bufLen)
{
	//Toledo* dev = (Toledo*)_dev;
	if(strcmp("getweight", func) == 0)
	{
		const unsigned char cmd[] = { 0x05 };
		int bWritten = bufLen;
		if(bWritten > 1)
			bWritten = 1;
		memcpy(cmdOut, cmd, bWritten);
		return bWritten;
	}
	if(strcmp("setprice", func) == 0)
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
	dev->test = Toledo_test;
	dev->getProperty = Toledo_getProperty;
	dev->makeCmd = Toledo_makeCmd;
	dev->free = Toledo_free;
	return dev;
}
