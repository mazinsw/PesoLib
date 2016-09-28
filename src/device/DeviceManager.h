/*
    PesoLib - Biblioteca para obtenção do peso de itens de uma balança
    Copyright (C) 2010-2014 MZSW Creative Software

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    MZSW Creative Software
    contato@mzsw.com.br
*/

/** @file DeviceManager.h
 *  device manager header for the PesoLib library
 */

#ifndef _DEVICEMANAGER_H_
#define _DEVICEMANAGER_H_
#include "Device.h"

typedef struct DeviceManager DeviceManager;

DeviceManager* DeviceManager_create();

int DeviceManager_getDeviceCount(DeviceManager* devman);

Device* DeviceManager_getDevice(DeviceManager* devman, int index);

int DeviceManager_addDevice(DeviceManager* devman, Device* dev);

const char* DeviceManager_getNames(DeviceManager* devman);
const char* DeviceManager_getModels(DeviceManager* devman, const char * deviceName);

void DeviceManager_free(DeviceManager* devman);

#endif /* _DEVICEMANAGER_H_ */