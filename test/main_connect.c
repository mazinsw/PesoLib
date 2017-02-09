#include <stdio.h>
#include <stdlib.h>
#include <PesoLib.h>
#include <windows.h>

typedef PesoLib * (LIBCALL * PesoLib_criaFunc)(const char*);
typedef void (LIBCALL * PesoLib_liberaFunc)(PesoLib*);

#ifdef _WIN64
#define DLL_FILE "../bin/x64/PesoLib.dll"
#else
#define DLL_FILE "../bin/x86/PesoLib.dll"
#endif

int main(int argc, char *argv[])
{
	HMODULE hModule = LoadLibrary(DLL_FILE);
	PesoLib_criaFunc _PesoLib_cria;
	PesoLib_liberaFunc _PesoLib_libera;
	if(hModule == 0)
		return 1;
	_PesoLib_cria = (PesoLib_criaFunc)GetProcAddress(hModule, "PesoLib_cria");
	_PesoLib_libera = (PesoLib_liberaFunc)GetProcAddress(hModule, "PesoLib_libera");

	PesoLib* lib = _PesoLib_cria(NULL);
	system("pause");
	_PesoLib_libera(lib);
	FreeLibrary(hModule);
	return 0;
}
