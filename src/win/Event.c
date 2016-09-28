#include "Event.h"
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

struct Event
{
	HANDLE hEvent;
};

Event* Event_create()
{
	return Event_createEx(TRUE, FALSE);
}

Event* Event_createEx(int manualReset, int signaled)
{
	Event* event = (Event*)malloc(sizeof(Event));
	event->hEvent = CreateEvent(NULL, manualReset, signaled, NULL);
	return event;
}

void Event_reset(Event* event)
{
	ResetEvent(event->hEvent);
}

void Event_post(Event* event)
{
	SetEvent(event->hEvent);
}

void Event_wait(Event* event)
{
	Event_waitEx(event, INFINITE);
}

Event* Event_waitMultiple(Event** events, int count)
{
	return Event_waitMultipleEx(events, count, INFINITE);
}

void Event_waitEx(Event* event, int milliseconds)
{
	WaitForSingleObject(event->hEvent, milliseconds);
}

Event* Event_waitMultipleEx(Event** events, int count, int milliseconds)
{
	HANDLE* handles = (HANDLE*)malloc(sizeof(HANDLE) * count);
	int i;
	DWORD signaled;
	
	for(i = 0; i < count; i++)
		handles[i] = (HANDLE*)Event_getHandle(events[i]);
	signaled = WaitForMultipleObjects(count, handles, FALSE, milliseconds);
	free(handles);
	if(signaled >= WAIT_OBJECT_0 && (int)signaled <= WAIT_OBJECT_0 + count)
		return events[signaled - WAIT_OBJECT_0];
	return NULL;
}

void* Event_getHandle(Event* event)
{
	return (void*)event->hEvent;
}

void Event_free(Event* event)
{
	Event_post(event);
	CloseHandle(event->hEvent);
	free(event);
}
