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

/** @file Device.h
 *  device header for the PesoLib library
 */

#ifndef _DEVICE_H_
#define _DEVICE_H_

typedef struct Device Device;

int Device_init(Device * dev);
int Device_execute(Device * dev, const unsigned char* buffer, int size);
int Device_test(Device * dev, const unsigned char* buffer, int size);
const char* Device_getProperty(Device * dev, const char* key);
int Device_makeCmd(Device * dev, const char* func, const char * data, 
	unsigned char* cmdOut, int bufLen);
void Device_free(Device * dev);

#endif /* _DEVICE_H_ */