#include "Mutex.h"
#include <windows.h>
#include <stdlib.h>

struct Mutex
{
	CRITICAL_SECTION criticalSection;
};

Mutex* Mutex_create()
{
	Mutex* mutex = (Mutex*)malloc(sizeof(Mutex));
	InitializeCriticalSection(&mutex->criticalSection);
	return mutex;
}

void Mutex_lock(Mutex* mutex)
{
	EnterCriticalSection(&mutex->criticalSection);
}

void Mutex_unlock(Mutex* mutex)
{
	LeaveCriticalSection(&mutex->criticalSection);
}

void Mutex_free(Mutex* mutex)
{
	DeleteCriticalSection(&mutex->criticalSection);
	free(mutex);
}
