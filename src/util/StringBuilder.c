#include "StringBuilder.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#define DEFAULT_ALLOC 256

typedef struct BufferItem
{
	char * buffer;
	int length;
	int allocated;
	struct BufferItem * next;
} BufferItem;

struct StringBuilder
{
	char * data;
	int length;
	unsigned char changed;
	BufferItem * buffer;
	BufferItem * last;
};

BufferItem* BufferItem_new(const char* buffer, int length, int allocate)
{
	BufferItem* item;
	item = (BufferItem*)malloc(sizeof(BufferItem));
	item->length = length;
	item->allocated = allocate;
	if(item->length > item->allocated)
		item->allocated = 1.2 * item->length;
	item->buffer = (char*)malloc(item->allocated);
	memcpy(item->buffer, buffer, item->length);
	item->next = NULL;
	return item;
}

void BufferItem_free(BufferItem* item)
{
	BufferItem *save, *next = item;
	while(next != NULL)
	{
		save = next->next;
		free(next->buffer);
		free(next);
		next = save;
	}
}

StringBuilder* StringBuilder_create()
{
	StringBuilder* builder;
	builder = (StringBuilder*)malloc(sizeof(StringBuilder));
	builder->data = (char*)malloc(1);
	builder->data[0] = 0;
	builder->length = 0;
	builder->buffer = NULL;
	builder->last = NULL;
	builder->changed = 0;
	return builder;
}

static void _StringBuilder_append(StringBuilder* builder, const char* str, int length)
{
	// empty buffer
	if(builder->buffer == NULL)
	{
		builder->buffer = BufferItem_new(str, length, DEFAULT_ALLOC);
		builder->last = builder->buffer;
	}
	else
	{
		// get available buffer
		int available = builder->last->allocated - builder->last->length;
		// not enougth buffer
		if(length > available)
		{
			// copy available
			memcpy(builder->last->buffer + builder->last->length, str, available);
			builder->last->length += available;
			// create new buffer
			int remaining = length - available;
			builder->last->next = BufferItem_new(str + available, remaining, DEFAULT_ALLOC);
			builder->last = builder->last->next;
		}
		else
		{
			// sufficient buffer
			memcpy(builder->last->buffer + builder->last->length, str, length);
			builder->last->length += length;
		}
	}
	// update status
	builder->length += length;
	builder->changed = 1;
}

void StringBuilder_append(StringBuilder* builder, const char* str)
{
	_StringBuilder_append(builder, str, strlen(str));
}

void StringBuilder_appendFormat(StringBuilder* builder, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	size_t size = DEFAULT_ALLOC;
	int nsize = 0;
	char* buffer;

	buffer = (char*)malloc(size);
	nsize = vsnprintf(buffer, size, format, args);
	if((int)size <= nsize) //fail delete buffer and try again
	{
		free(buffer);
		size = nsize;
		buffer = (char*)malloc(size + 1);
		nsize = vsnprintf(buffer, size, format, args);
	}
	va_end(args);
	_StringBuilder_append(builder, buffer, nsize);
	free(buffer);
}

const char * StringBuilder_getData(StringBuilder* builder)
{
	if(builder->changed == 0)
		return builder->data;
	free(builder->data);
	builder->data = (char*)malloc(builder->length + 1);
	char * ptr = builder->data;
	BufferItem * next = builder->buffer;
	while(next != NULL)
	{
		memcpy(ptr, next->buffer, next->length);
		ptr += next->length;
		next = next->next;
	}
	builder->data[builder->length] = 0;
	builder->changed = 0;
	return builder->data;
}

int StringBuilder_getLength(StringBuilder* builder)
{
	return builder->length;
}

void StringBuilder_clear(StringBuilder* builder)
{
	BufferItem_free(builder->buffer);
	builder->buffer = NULL;
	builder->last = NULL;
	if(builder->length != 0)
	{
		free(builder->data);
		builder->data = (char*)malloc(1);
		builder->data[0] = 0;
		builder->length = 0;
		builder->changed = 0;
	}
}

void StringBuilder_free(StringBuilder* builder)
{
	BufferItem_free(builder->buffer);
	free(builder->data);
	free(builder);
}
