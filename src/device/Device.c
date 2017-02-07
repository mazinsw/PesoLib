#include "Device.h"
#include "DevicePrivate.h"
#include <stdlib.h>

Device * Device_alloc(void * data)
{
	Device * dev = (Device*)malloc(sizeof(Device));
	dev->data = data;
	return dev;
}

void Device_release(Device * dev)
{
	free(dev);
}

int Device_init(Device* dev)
{
	return dev->init(dev);
}

int Device_execute(Device* dev, const unsigned char* buffer, int size)
{
	return dev->execute(dev, buffer, size);
}

int Device_isStable(Device * dev)
{
	return dev->isStable(dev);
}

int Device_getWeight(Device * dev)
{
	return dev->getWeight(dev);
}

void Device_getResponseRange(Device * dev, int * min, int * max)
{
	return dev->getResponseRange(dev, min, max);
}

const char* Device_getName(Device * dev)
{
	return dev->getName(dev);
}

const char* Device_getProperty(Device* dev, const char* key)
{
	return dev->getProperty(dev, key);
}

int Device_makeCmd(Device* dev, const char* func, const char * data, 
	unsigned char* cmdOut, int bufLen)
{
	return dev->makeCmd(dev, func, data, cmdOut, bufLen);
}

void Device_free(Device* dev)
{
	dev->free(dev);
}
