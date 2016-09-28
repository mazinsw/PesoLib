#include "Filizola.h"
#include "DevicePrivate.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct Filizola
{
	int peso;
	char models[48];
	char buffer[16];
} Filizola;


int Filizola_init(Device* _dev)
{
	Filizola* dev = (Filizola*)_dev->data;
	dev->peso = 0;
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
	*stable = 1;
	return 7;
}

int Filizola_execute(Device* _dev, const unsigned char* buffer, int size)
{
	Filizola* dev = (Filizola*)_dev->data;
	int peso, stable = 0;
	int used = _Filizola_execute(buffer, size, &peso, &stable);
	if(used == 0 || stable != 1)
		return 0;
	dev->peso = peso;
	return used;
}

int Filizola_test(Device* _dev, const unsigned char* buffer, int size)
{
	int peso, stable;
	return _Filizola_execute(buffer, size, &peso, &stable);
}

const char* Filizola_getProperty(Device* _dev, const char* key)
{
	Filizola* dev = (Filizola*)_dev->data;
	if(strcmp("weight", key) == 0)
	{
		sprintf(dev->buffer, "%d", dev->peso);
		return dev->buffer;
	}
	if(strcmp("name", key) == 0)
	{
		strcpy(dev->buffer, "Filizola");
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

int Filizola_makeCmd(Device* _dev, const char* func, const char * data, 
	unsigned char* cmdOut, int bufLen)
{
	//Filizola* dev = (Filizola*)_dev;
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
	dev->test = Filizola_test;
	dev->getProperty = Filizola_getProperty;
	dev->makeCmd = Filizola_makeCmd;
	dev->free = Filizola_free;
	return dev;
}
