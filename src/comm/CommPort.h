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

/** @file CommPort.h
 *  com port header for the PesoLib library
 */

#ifndef _COMMPORT_H_
#define _COMMPORT_H_
#include "Event.h"

typedef struct CommPort CommPort;

typedef enum CommParity
{
	Parity_None = 0,
	Parity_Odd,
	Parity_Even,
	Parity_Mark,
	Parity_Space
} CommParity;

typedef enum CommFlow
{
	Flow_None = 0,
	Flow_XONXOFF,
	Flow_RTSCTS,
	Flow_DSRDTR
} CommFlow;

typedef enum CommStopBits
{
	StopBits_One = 0,
	StopBits_OneAndHalf,
	StopBits_Two
} CommStopBits;

typedef struct CommSettings
{
	int baund;
	CommParity parity;
	unsigned char data;
	CommStopBits stop;
	CommFlow flow;
} CommSettings;

CommPort* CommPort_create(const char* port);
CommPort* CommPort_createEx(const char* port, const CommSettings* settings);
int CommPort_configure(CommPort* comm, const CommSettings* settings);
int CommPort_enum(char * buffer, int size);
int CommPort_write(CommPort* comm, const unsigned char * bytes, int count);
int CommPort_writeEx(CommPort* comm, const unsigned char * bytes, int count, int milliseconds);
int CommPort_wait(CommPort* comm, int * bytesAvailable);
int CommPort_waitEx(CommPort* comm, int * bytesAvailable, int milliseconds);
int CommPort_read(CommPort* comm, unsigned char * bytes, int count);
int CommPort_readEx(CommPort* comm, unsigned char * bytes, int count, int milliseconds);
void CommPort_cancel(CommPort* comm);
void CommPort_free(CommPort* comm);

#endif /* _COMMPORT_H_ */