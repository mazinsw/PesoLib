#include "DeviceManager.h"
#include "Filizola.h"
#include "Toledo.h"
#include "Alfa.h"
#include "Magna.h"
#include "Urano.h"
#include "Elgin.h"
#include "StringBuilder.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX 16

struct DeviceManager
{
	Device * devs[MAX];
	int count;
	StringBuilder* names;
	StringBuilder* models;
};

DeviceManager* DeviceManager_create()
{
	DeviceManager* devman = (DeviceManager*)malloc(sizeof(DeviceManager));
	devman->count = 0;
	devman->names = StringBuilder_create();
	devman->models = StringBuilder_create();
	/* Add devices here */
	
	DeviceManager_addDevice(devman, Device_createToledo());
	DeviceManager_addDevice(devman, Device_createFilizola());
	DeviceManager_addDevice(devman, Device_createUrano());
	DeviceManager_addDevice(devman, Device_createElgin());
	DeviceManager_addDevice(devman, Device_createAlfa());
	DeviceManager_addDevice(devman, Device_createMagna());
	
	/* End add devices */
	int i;
	for(i = 0; i < devman->count; i++)
	{
		Device * dev = DeviceManager_getDevice(devman, i);
		if(i > 0)
			StringBuilder_append(devman->names, "\r\n");
		StringBuilder_append(devman->names, Device_getName(dev));
	}
	return devman;
}

const char* DeviceManager_getNames(DeviceManager* devman)
{
	return StringBuilder_getData(devman->names);
}

const char* DeviceManager_getModels(DeviceManager* devman, const char * deviceName)
{
	StringBuilder_clear(devman->models);
	int i;
	for(i = 0; i < devman->count; i++)
	{
		Device * dev = DeviceManager_getDevice(devman, i);
		if(strcmp(deviceName, Device_getName(dev)) != 0)
			continue;
		StringBuilder_append(devman->models, Device_getProperty(dev, DEV_PROP_MODELS));
		break;
	}
	return StringBuilder_getData(devman->models);
}

int DeviceManager_getDeviceCount(DeviceManager* devman)
{
	return devman->count;
}

Device* DeviceManager_getDevice(DeviceManager* devman, int index)
{
	if(index < 0 || index >= devman->count)
		return NULL;
	return devman->devs[index];
}

int DeviceManager_addDevice(DeviceManager* devman, Device* dev)
{
	if(devman->count == MAX)
		return -1;
	devman->devs[devman->count] = dev;
	devman->count++;
	Device_init(dev);
	return devman->count - 1;
}

void DeviceManager_free(DeviceManager* devman)
{
	int i;
	for(i = devman->count - 1; i >= 0; i--)
	{
		Device_free(devman->devs[i]);
		devman->count--;
	}
	StringBuilder_free(devman->models);
	StringBuilder_free(devman->names);
	free(devman);
}
