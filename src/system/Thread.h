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

/** @file Thread.h
 *  thread file header for the PesoLib library
 */

#ifndef _THREAD_H_
#define _THREAD_H_

typedef struct Thread Thread;
typedef void (*ThreadFunc)(void*);

Thread* Thread_create(ThreadFunc fn, void* data);
void Thread_start(Thread* thread);
void Thread_join(Thread* thread);
void Thread_wait(int milliseconds);
void Thread_free(Thread* thread);

#endif /* _THREAD_H_ */