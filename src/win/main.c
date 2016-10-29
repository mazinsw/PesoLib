#include <private/Plataforma.h>
#include <windows.h>

#ifdef BUILD_DLL
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch(fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		return PesoLib_inicializa();
	case DLL_PROCESS_DETACH:
		PesoLib_finaliza();
		break;
	}
	return TRUE; // succesful
}
#endif

#if !defined(BUILD_DLL) && !defined(LIB_STATIC)
#include "PesoLib.h"
#include "CommPort.h"
#include <stdio.h>

int main(int argc, char** argv)
{
	DWORD startTick = GetTickCount();
	int gramas;

	PesoLib_inicializa();
	PesoLib* lib = PesoLib_cria(NULL);
	int evento;
	DWORD endTick;
	while((evento = PesoLib_aguardaEvento(lib)) != Evento_Cancelado)
	{
		switch(evento)
		{
		case Evento_Conectado:
			endTick = GetTickCount() - startTick;
			printf("Balanca conectada\n");
			printf("Tempo: %.3lf s\n", endTick / 1000.0);
			PesoLib_solicitaPeso(lib, 5.00f);
			continue;
		case Evento_Desconectado:
			printf("Balanca desconectada\n");
			startTick = GetTickCount();
			Sleep(5000);
			continue;
		}
		gramas = PesoLib_getUltimoPeso(lib);
		printf("Peso recebido: %.3f Kg\n", gramas / 1000.0);
	}
	PesoLib_libera(lib);
	PesoLib_finaliza();
	return 0;
}
#endif