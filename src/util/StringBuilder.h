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

/** @file StringBuilder.h
 *  string builder header for the PesoLib library
 */

#ifndef _STRINGBUILDER_H_
#define _STRINGBUILDER_H_

/** Data type of string builder, should be used as pointer */
typedef struct StringBuilder StringBuilder;

/**
 * Create a string builder instance
 * 
 */
StringBuilder* StringBuilder_create();

/**
 * Append string to end of buffer
 * 
 * parameters
 *   builder: instance of buffer
 *   str: string to append
 */
void StringBuilder_append(StringBuilder* builder, const char * str);

/**
 * Append a formatted string to end of buffer
 * 
 * parameters
 *   builder: instance of buffer
 *   format: string format
 *   ...: values of format
 */
void StringBuilder_appendFormat(StringBuilder* builder, const char * format, ...);

/**
 * Get string of all appended data
 * 
 * parameters
 *   builder: instance of buffer
 * return
 *   null terminated string with string builder content
 */
const char * StringBuilder_getData(StringBuilder* builder);

/**
 * Return the length of string
 * 
 * parameters
 *   builder: instance of buffer
 * return
 *   length of string
 */
int StringBuilder_getLength(StringBuilder* builder);

/**
 * Clear all append string, turn empty buffer
 * 
 * parameters
 *   builder: instance of buffer to clear
 */
void StringBuilder_clear(StringBuilder* builder);

/**
 * Free buffer instance
 * 
 * parameters
 *   builder: instance of buffer to free
 */
void StringBuilder_free(StringBuilder* builder);

#endif /* _STRINGBUILDER_H_ */