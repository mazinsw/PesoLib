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

/** @file Event.h
 *  event file header for the PesoLib library
 */

#ifndef _EVENT_H_
#define _EVENT_H_

typedef struct Event Event;

Event* Event_create();
Event* Event_createEx(int manualReset, int signaled);
void Event_post(Event* event);
void Event_reset(Event* event);
void Event_wait(Event* event);
Event* Event_waitMultiple(Event** events, int count);
void Event_waitEx(Event* event, int milliseconds);
Event* Event_waitMultipleEx(Event** events, int count, int milliseconds);
void* Event_getHandle(Event* event);
void Event_free(Event* event);

#endif /* _EVENT_H_ */