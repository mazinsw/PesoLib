#include <stdio.h>
#include <stdlib.h>
#include <PesoLib.h>
#include <windows.h>

#ifdef _WIN64
#define DLL_FILE "../bin/x64/PesoLib.dll"
#else
#define DLL_FILE "../bin/x86/PesoLib.dll"
#endif

int main(int argc, char *argv[])
{
	HMODULE hModule = LoadLibrary(DLL_FILE);
	if(hModule == 0)
		return 1;
	FreeLibrary(hModule);
	return 0;
}
