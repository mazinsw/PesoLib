#include "Alfa.h"
#include "DevicePrivate.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct Alfa
{
	int peso;
	char models[48];
	char buffer[24];
} Alfa;


int Alfa_init(Device* _dev)
{
	Alfa* dev = (Alfa*)_dev->data;
	dev->peso = 0;
	strcpy(dev->models, "3101C"); // TODO: add models
	return 1;
}

static int _Alfa_execute(const unsigned char* buffer, int size,
						  int * peso, int * stable)
{
	// PB: 230,50 T: 000,00\r\n
	// PL: 123,45 T: 010,00\r\n
	// **: 230,50 T: 000,00\r\n
	if(size < 22 || buffer[2] != ':' || buffer[11] != 'T' || buffer[12] != ':'
			|| buffer[20] != 0x13 || buffer[21] != 0x10)
		return 0;
	if(buffer[0] != 'P' || buffer[1] != 'L' || buffer[3] == '-')
	{
		if((buffer[0] == '*' && buffer[1] == '*') ||
				(buffer[0] == 'P' || buffer[1] == 'B'))
		{
			*stable = 0;
			return 22;
		}
		return 0;
	}
	int i, _peso = 0, mult = 1;
	unsigned char bff[6];
	memcpy(bff, buffer + 4, 3);
	memcpy(bff, buffer + 8, 2);
	bff[5] = '0';
	for(i = 5; i >= 0; i--)
	{
		if(bff[i] < '0' || bff[i] > '9')
			return 0;
		_peso += mult * (bff[i] - '0');
		mult *= 10;
	}
	*peso = _peso;
	*stable = 1;
	return 22;
}

int Alfa_execute(Device* _dev, const unsigned char* buffer, int size)
{
	Alfa* dev = (Alfa*)_dev->data;
	int peso, stable = 0;
	int used = _Alfa_execute(buffer, size, &peso, &stable);
	if(used == 0 || stable != 1)
		return 0;
	dev->peso = peso;
	return used;
}

int Alfa_test(Device* _dev, const unsigned char* buffer, int size)
{
	int peso, stable;
	return _Alfa_execute(buffer, size, &peso, &stable);
}

const char* Alfa_getProperty(Device* _dev, const char* key)
{
	Alfa* dev = (Alfa*)_dev->data;
	if(strcmp("weight", key) == 0)
	{
		sprintf(dev->buffer, "%d", dev->peso);
		return dev->buffer;
	}
	if(strcmp("name", key) == 0)
	{
		strcpy(dev->buffer, "Alfa");
		return dev->buffer;
	}
	if(strcmp("models", key) == 0)
	{
		return dev->models;
	}
	if(strcmp("response_size", key) == 0)
	{
		static const char * rsize = "22";
		return rsize;
	}
	return NULL;
}

int Alfa_makeCmd(Device* _dev, const char* func, const char * data,
				  unsigned char* cmdOut, int bufLen)
{
	//Alfa* dev = (Alfa*)_dev;
	if(strcmp("getweight", func) == 0)
	{
		const unsigned char cmd[] = { '0', '1', 'P', 0x13, 0x10 };
		int bWritten = bufLen;
		if(bWritten > sizeof(cmd))
			bWritten = sizeof(cmd);
		memcpy(cmdOut, cmd, bWritten);
		return bWritten;
	}
	if(strcmp("setprice", func) == 0)
	{
		return 0; // not supported
	}
	return 0;
}

void Alfa_free(Device* _dev)
{
	Alfa* dev = (Alfa*)_dev->data;
	free(dev);
	Device_release(_dev);
}

Device * Device_createAlfa()
{
	Alfa* _dev = (Alfa*)malloc(sizeof(Alfa));
	Device* dev = Device_alloc(_dev);
	dev->init = Alfa_init;
	dev->execute = Alfa_execute;
	dev->test = Alfa_test;
	dev->getProperty = Alfa_getProperty;
	dev->makeCmd = Alfa_makeCmd;
	dev->free = Alfa_free;
	return dev;
}
