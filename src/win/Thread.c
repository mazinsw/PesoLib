#include "Thread.h"
#include <windows.h>
#include <stdlib.h>

struct Thread
{
	void * data;
	ThreadFunc fn;
	HANDLE hThread;
};

Thread* Thread_create(ThreadFunc fn, void* data)
{
	Thread* thread = (Thread*)malloc(sizeof(Thread));
	thread->hThread = 0;
	thread->fn = fn;
	thread->data = data;
	return thread;
}

static DWORD WINAPI threadProc(LPVOID param)
{
	if(((Thread*)param)->fn != 0)
		((Thread*)param)->fn(((Thread*)param)->data);
	((Thread*)param)->hThread = 0;
	return 0;
}

void Thread_start(Thread* thread)
{
	if(thread->hThread != 0)
		return;
	thread->hThread = CreateThread(NULL, 0, threadProc,
						   (void*)thread, 0, NULL);
}

void Thread_join(Thread* thread)
{
	if(thread->hThread == 0)
		return;
	WaitForSingleObject(thread->hThread, INFINITE);
}

void Thread_wait(int milliseconds)
{
	Sleep(milliseconds);
}

void Thread_free(Thread* thread)
{
	if(thread->hThread != 0)
		TerminateThread(thread->hThread, 0);
	free(thread);
}
