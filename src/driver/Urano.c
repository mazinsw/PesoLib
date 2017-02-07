#include "Urano.h"
#include "DevicePrivate.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

typedef struct Urano
{
	int stable;
	int weight;
	char models[48];
	char buffer[24];
} Urano;

int Urano_init(Device* _dev)
{
	Urano* dev = (Urano*)_dev->data;
	dev->stable = 0;
	dev->weight = 0;
	strcpy(dev->buffer, "Urano");
	strcpy(dev->models, "UDC30000/5"); // TODO: add models
	return 1;
}

static int _Urano_execute_urano_c(const unsigned char* buffer, int size,
						  int * peso, int * stable)
{
	// Urano12: ?
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
	*stable = _peso > 0;
	return 69;
}

static int _Urano_execute_urano_pop(const unsigned char* buffer, int size,
						  int * peso, int * stable)
{
	// USE-PII(Sem data):[ESC]T2[ESC]B[ESC]N0           0,166 kg     34,90[ESC]N1     5,79[ESC]E[ESC]P1
	if(size < 56 || buffer[0] != 0x1B || buffer[1] != 'T' || buffer[2] != '2' || 
			buffer[53] != 0x1B || buffer[54] != 'P' || buffer[55] != '1')
		return 0;
	int i, _peso = 0, mult = 1;
	for(i = 23; i >= 18; i--)
	{
		if(i == 20 && buffer[i] == ',')
		{
			continue;
		} else if(i == 18 && buffer[i] == ' ')
			continue;
		if(buffer[i] < '0' || buffer[i] > '9')
			return 0;
		_peso += mult * (buffer[i] - '0');
		mult *= 10;
	}
	*peso = _peso;
	*stable = _peso > 0;
	return 56;
}

int Urano_execute(Device* _dev, const unsigned char* buffer, int size)
{
	Urano* dev = (Urano*)_dev->data;
	int r = _Urano_execute_urano_c(buffer, size, &dev->weight, &dev->stable);
	if(r != 0)
		return r;
	return _Urano_execute_urano_pop(buffer, size, &dev->weight, &dev->stable);
}

int Urano_isStable(Device * _dev)
{
	Urano* dev = (Urano*)_dev->data;
	return dev->stable;
}

int Urano_getWeight(Device * _dev)
{
	Urano* dev = (Urano*)_dev->data;
	return dev->weight;
}

void Urano_getResponseRange(Device * _dev, int * min, int * max)
{
	*min = 56;
	*max = 69;
}

const char* Urano_getName(Device * _dev)
{
	Urano* dev = (Urano*)_dev->data;
	return dev->buffer;
}

const char* Urano_getProperty(Device* _dev, const char* key)
{
	Urano* dev = (Urano*)_dev->data;
	if(strcmp(DEV_PROP_MODELS, key) == 0)
	{
		return dev->models;
	}
	return NULL;
}

int Urano_makeCmd(Device* _dev, const char* func, const char * data,
				  unsigned char* cmdOut, int bufLen)
{
	//Urano* dev = (Urano*)_dev;
	if(strcmp(DEV_CMD_GET_WEIGHT, func) == 0)
	{
		const unsigned char cmd[] = { 0x04 };
		int bWritten = bufLen;
		if(bWritten > sizeof(cmd))
			bWritten = sizeof(cmd);
		memcpy(cmdOut, cmd, bWritten);
		return bWritten;
	}
	if(strcmp(DEV_CMD_SET_PRICE, func) == 0)
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
	dev->isStable = Urano_isStable;
	dev->getWeight = Urano_getWeight;
	dev->getResponseRange = Urano_getResponseRange;
	dev->getName = Urano_getName;
	dev->getProperty = Urano_getProperty;
	dev->makeCmd = Urano_makeCmd;
	dev->free = Urano_free;
	return dev;
}
