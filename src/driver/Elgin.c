#include "Elgin.h"
#include "DevicePrivate.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct Elgin
{
	int weight;
	int stable;
	char models[48];
	char buffer[16];
} Elgin;

int Elgin_init(Device* _dev)
{
	Elgin* dev = (Elgin*)_dev->data;
	dev->weight = 0;
	dev->stable = 0;
	strcpy(dev->buffer, "Elgin");
	strcpy(dev->models, ""); // TODO: add models
	return 1;
}

static int _Elgin_execute_normal(const unsigned char* buffer, int size,
	int * weight, int * stable)
{
	int i;
	int _peso = 0, mult = 1;
	
	if(size >= 8 && buffer[0] == 0x02 && buffer[7] == 0x03)
	{
		// STX IIIIII ETX | STX NNNNNN ETX | STX SSSSSS ETX
		if(buffer[1] == 'I' || buffer[1] == 'N' || buffer[1] == 'S')
		{
			for(i = 6; i >= 2; i--)
			{
				if(buffer[1] != buffer[i])
					return 0;
			}
			*stable = 0;
			return 8;
		}
	}
	
	if(size < 9 || buffer[0] != 0x02 || buffer[8] != 0x03)
		return 0;
	// STX 00.250ETX
	for(i = 7; i >= 1; i--)
	{
		if(i == 4 && buffer[i] == '.')
			continue;
		if(i == 1 && buffer[i] == '-')
		{
			*stable = 0;
			return 9;
		} else if(i == 1)
			break;
		if(buffer[i] < '0' || buffer[i] > '9')
			return 0;
		_peso += mult * (buffer[i] - '0');
		mult *= 10;
	}
	*weight = _peso;
	*stable = _peso > 0;
	return 9;
}

// PESO/PREÇO/TOTAL1 (FILIZOLA CS E PLURIS)
static int _Elgin_execute_avancado(const unsigned char* buffer, int size,
	int * weight, int * stable)
{
	int i;
	int _peso = 0, mult = 1;
	
	if(size < 22 || buffer[0] != 0x02 || buffer[21] != 0x03)
		return 0;
	// [STX]IIIIIIIIIIIIIIIIIIII[ETX]
	// [STX]NNNNNNNNNNNNNNNNNNNN[ETX]
	// [STX]SSSSSSSSSSSSSSSSSSSS[ETX]
	if(buffer[1] == 'I' || buffer[1] == 'N' || buffer[1] == 'S')
	{
		for(i = 20; i >= 2; i--)
		{
			if(buffer[1] != buffer[i])
				return 0;
		}
		*stable = 0;
		return 22;
	}
	// [STX] 00666    000    000[ETX]
	for(i = 6; i >= 1; i--)
	{
		if(i == 4 && buffer[i] == '.')
			continue;
		if(i == 1 && buffer[i] == '-')
		{
			*stable = 0;
			return 22;
		} else if(i == 1)
			break;
		if(buffer[i] < '0' || buffer[i] > '9')
			return 0;
		_peso += mult * (buffer[i] - '0');
		mult *= 10;
	}
	*weight = _peso;
	*stable = _peso > 0;
	return 22;
}

int Elgin_execute(Device* _dev, const unsigned char* buffer, int size)
{
	Elgin* dev = (Elgin*)_dev->data;
	int r = _Elgin_execute_normal(buffer, size, &dev->weight, &dev->stable);
	if(r != 0)
		return r;
	return _Elgin_execute_avancado(buffer, size, &dev->weight, &dev->stable);
}

int Elgin_isStable(Device * _dev)
{
	Elgin* dev = (Elgin*)_dev->data;
	return dev->stable;
}

int Elgin_getWeight(Device * _dev)
{
	Elgin* dev = (Elgin*)_dev->data;
	return dev->weight;
}

void Elgin_getResponseRange(Device * _dev, int * min, int * max)
{
	*min = 8;
	*max = 22;
}

const char* Elgin_getName(Device * _dev)
{
	Elgin* dev = (Elgin*)_dev->data;
	return dev->buffer;
}

const char* Elgin_getProperty(Device* _dev, const char* key)
{
	Elgin* dev = (Elgin*)_dev->data;
	if(strcmp(DEV_PROP_MODELS, key) == 0)
	{
		return dev->models;
	}
	return NULL;
}

int Elgin_makeCmd(Device* _dev, const char* func, const char * data, 
	unsigned char* cmdOut, int bufLen)
{
	//Elgin* dev = (Elgin*)_dev;
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

void Elgin_free(Device* _dev)
{
	Elgin* dev = (Elgin*)_dev->data;
	free(dev);
	Device_release(_dev);
}

Device * Device_createElgin()
{
	Elgin * _dev = (Elgin*)malloc(sizeof(Elgin));
	Device * dev = Device_alloc(_dev);
	dev->init = Elgin_init;
	dev->execute = Elgin_execute;
	dev->isStable = Elgin_isStable;
	dev->getWeight = Elgin_getWeight;
	dev->getResponseRange = Elgin_getResponseRange;
	dev->getName = Elgin_getName;
	dev->getProperty = Elgin_getProperty;
	dev->makeCmd = Elgin_makeCmd;
	dev->free = Elgin_free;
	return dev;
}
