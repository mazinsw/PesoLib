#include <stdio.h>
#include <stdlib.h>
#include <PesoLib.h>
#include <windows.h>

int main(int argc, char *argv[])
{
	HMODULE hModule = LoadLibrary("../bin/PesoLib.dll");
	if(hModule == 0)
		return 1;
	FreeLibrary(hModule);
	return 0;
}
