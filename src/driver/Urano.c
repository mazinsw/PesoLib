#include "Urano.h"
#include "DevicePrivate.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

typedef struct Urano
{
	int peso;
	char models[48];
	char buffer[24];
} Urano;


int Urano_init(Device* _dev)
{
	Urano* dev = (Urano*)_dev->data;
	dev->peso = 0;
	strcpy(dev->models, "UDC30000/5"); // TODO: add models
	return 1;
}

static int _Urano_execute(const unsigned char* buffer, int size,
						  int * peso, int * stable)
{
	// Urano13: ?
	// UranoC : [ESC]T2[ESC]A13[ESC]N0[ESC]S2[ESC]D4[ESC]Q193[ESC]B * PESO(L):    135 g      TARA:      0 g[ESC]E[ESC]P01
	if(size < 69 || buffer[0] != 0x1B || buffer[1] != 'T' || buffer[2] != '2' || 
			buffer[65] != 0x1B || buffer[66] != 'P' || buffer[67] != '0' || buffer[68] != '1')
		return 0;
	if(buffer[24] != '*')
	{
		*stable = 0;
		return 69;
	}
	int i, _peso = 0, mult = 1;
	unsigned char bff[6];
	char kilo = buffer[41] == 'k';
	memcpy(bff, buffer + 35, 6);
	for(i = 5; i >= 0; i--)
	{
		if(bff[i] == ',')
		{
			if(kilo)
				_peso *= pow(10, 3 - (5 - i));
			else
			{
				_peso = 0;
				mult = 1;
			}
			continue;
		} else if(bff[i] == ' ')
			continue;
		if(bff[i] < '0' || bff[i] > '9')
			return 0;
		_peso += mult * (bff[i] - '0');
		mult *= 10;
	}
	*peso = _peso;
	*stable = 1;
	return 69;
}

int Urano_execute(Device* _dev, const unsigned char* buffer, int size)
{
	Urano* dev = (Urano*)_dev->data;
	int peso, stable = 0;
	int used = _Urano_execute(buffer, size, &peso, &stable);
	if(used == 0 || stable != 1)
		return 0;
	dev->peso = peso;
	return used;
}

int Urano_test(Device* _dev, const unsigned char* buffer, int size)
{
	int peso, stable;
	return _Urano_execute(buffer, size, &peso, &stable);
}

const char* Urano_getProperty(Device* _dev, const char* key)
{
	Urano* dev = (Urano*)_dev->data;
	if(strcmp("weight", key) == 0)
	{
		sprintf(dev->buffer, "%d", dev->peso);
		return dev->buffer;
	}
	if(strcmp("name", key) == 0)
	{
		strcpy(dev->buffer, "Urano");
		return dev->buffer;
	}
	if(strcmp("models", key) == 0)
	{
		return dev->models;
	}
	if(strcmp("response_size", key) == 0)
	{
		static const char * rsize = "69";
		return rsize;
	}
	return NULL;
}

int Urano_makeCmd(Device* _dev, const char* func, const char * data,
				  unsigned char* cmdOut, int bufLen)
{
	//Urano* dev = (Urano*)_dev;
	if(strcmp("getweight", func) == 0)
	{
		const unsigned char cmd[] = { 0x04 };
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

void Urano_free(Device* _dev)
{
	Urano* dev = (Urano*)_dev->data;
	free(dev);
	Device_release(_dev);
}

Device * Device_createUrano()
{
	Urano* _dev = (Urano*)malloc(sizeof(Urano));
	Device* dev = Device_alloc(_dev);
	dev->init = Urano_init;
	dev->execute = Urano_execute;
	dev->test = Urano_test;
	dev->getProperty = Urano_getProperty;
	dev->makeCmd = Urano_makeCmd;
	dev->free = Urano_free;
	return dev;
}
