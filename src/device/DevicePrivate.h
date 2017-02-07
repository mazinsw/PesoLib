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

/** @file DevicePrivate.h
 *  private declaration for new drivers for the PesoLib library
 */

#ifndef _DEVICEPRIVATE_H_
#define _DEVICEPRIVATE_H_
#include "Device.h"

typedef int (*DeviceInitFunc)(Device *);
typedef int (*DeviceExecuteFunc)(Device *, const unsigned char*, int);
typedef int (*DeviceIsStableFunc)(Device * dev);
typedef int (*DeviceGetWeightFunc)(Device * dev);
typedef void (*DeviceGetResponseRangeFunc)(Device * dev, int * min, int * max);
typedef const char* (*DeviceGetNameFunc)(Device * dev);
typedef const char* (*DeviceGetPropertyFunc)(Device *, const char*);
typedef int (*DeviceMakeCmdFunc)(Device *, const char*, const char*, 
	unsigned char*, int);
typedef void (*DeviceFreeFunc)(Device *);

struct Device
{
	DeviceInitFunc init;
	DeviceExecuteFunc execute;
	DeviceIsStableFunc isStable;
	DeviceGetWeightFunc getWeight;
	DeviceGetResponseRangeFunc getResponseRange;
	DeviceGetNameFunc getName;
	DeviceGetPropertyFunc getProperty;
	DeviceMakeCmdFunc makeCmd;
	DeviceFreeFunc free;
	void * data;
};

Device * Device_alloc(void * data);
void Device_release(Device * dev);

#endif /* _DEVICEPRIVATE_H_ */