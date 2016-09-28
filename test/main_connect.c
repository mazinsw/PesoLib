#include <stdio.h>
#include <stdlib.h>
#include <PesoLib.h>
#include <windows.h>
typedef PesoLib * (LIBCALL * PesoLib_inicializaFunc)(const char*);
typedef void * (LIBCALL * PesoLib_finalizaFunc)(PesoLib*);

int main(int argc, char *argv[])
{
	HMODULE hModule = LoadLibrary("../bin/PesoLib.dll");
	PesoLib_inicializaFunc _PesoLib_inicializa;
	PesoLib_finalizaFunc _PesoLib_finaliza;
	if(hModule == 0)
		return 1;
	_PesoLib_inicializa = (PesoLib_inicializaFunc)GetProcAddress(hModule, "PesoLib_inicializa");
	_PesoLib_finaliza = (PesoLib_finalizaFunc)GetProcAddress(hModule, "PesoLib_finaliza");
	
	PesoLib* lib = _PesoLib_inicializa(NULL);
	system("pause");
	_PesoLib_finaliza(lib);
	FreeLibrary(hModule);
	return 0;
}
