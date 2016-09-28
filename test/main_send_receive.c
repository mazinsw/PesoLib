#include <stdio.h>
#include <stdlib.h>
#include <PesoLib.h>
#include <windows.h>

typedef PesoLib * (LIBCALL * PesoLib_criaFunc)(const char*);
typedef int (LIBCALL * PesoLib_getUltimoPesoFunc)(PesoLib *);
typedef int (LIBCALL * PesoLib_solicitaPesoFunc)(PesoLib *, float);
typedef int (LIBCALL * PesoLib_aguardaEventoFunc)(PesoLib *);
typedef void (LIBCALL * PesoLib_cancelaFunc)(PesoLib*);
typedef void (LIBCALL * PesoLib_liberaFunc)(PesoLib*);
typedef void (LIBCALL * PesoLib_setConfiguracaoFunc)(PesoLib*, const char*);
typedef const char * (LIBCALL * PesoLib_getConfiguracaoFunc)(PesoLib*);

int main(int argc, char *argv[])
{
	DWORD startTick = GetTickCount();
	HMODULE hModule = LoadLibrary("../bin/PesoLib.dll");
	PesoLib_criaFunc _PesoLib_cria;
	PesoLib_getUltimoPesoFunc _PesoLib_getUltimoPeso;
	PesoLib_solicitaPesoFunc _PesoLib_solicitaPeso;
	PesoLib_aguardaEventoFunc _PesoLib_aguardaEvento;
	PesoLib_setConfiguracaoFunc _PesoLib_setConfiguracao;
	PesoLib_getConfiguracaoFunc _PesoLib_getConfiguracao;
	PesoLib_cancelaFunc _PesoLib_cancela;
	PesoLib_liberaFunc _PesoLib_libera;
	if(hModule == 0)
		return 1;
	_PesoLib_cria = (PesoLib_criaFunc)GetProcAddress(hModule, "PesoLib_cria");
	_PesoLib_getUltimoPeso = (PesoLib_getUltimoPesoFunc)GetProcAddress(hModule, "PesoLib_getUltimoPeso");
	_PesoLib_solicitaPeso = (PesoLib_solicitaPesoFunc)GetProcAddress(hModule, "PesoLib_solicitaPeso");
	_PesoLib_aguardaEvento = (PesoLib_aguardaEventoFunc)GetProcAddress(hModule, "PesoLib_aguardaEvento");
	_PesoLib_setConfiguracao = (PesoLib_setConfiguracaoFunc)GetProcAddress(hModule, "PesoLib_setConfiguracao");
	_PesoLib_getConfiguracao = (PesoLib_getConfiguracaoFunc)GetProcAddress(hModule, "PesoLib_getConfiguracao");
	_PesoLib_cancela = (PesoLib_cancelaFunc)GetProcAddress(hModule, "PesoLib_cancela");
	_PesoLib_libera = (PesoLib_liberaFunc)GetProcAddress(hModule, "PesoLib_libera");
	int gramas;
	PesoLib* lib = _PesoLib_cria(NULL);
	int evento;
	DWORD endTick;
	while((evento = _PesoLib_aguardaEvento(lib)) != Evento_Cancelado)
	{
		switch(evento)
		{
		case Evento_Conectado:
			endTick = GetTickCount() - startTick;
			printf("Balanca conectada\n");
			printf("Tempo: %.3lf s\n", endTick / 1000.0);
			_PesoLib_solicitaPeso(lib, 5.00f);
			continue;
		case Evento_Desconectado:
			printf("Balanca desconectada\n");
			startTick = GetTickCount();
			Sleep(5000);
			continue;
		}
		gramas = _PesoLib_getUltimoPeso(lib);
		printf("Peso recebido: %.3f Kg\n", gramas / 1000.0);
		//_PesoLib_cancela(lib);
		//printf("Configuracao: %s\n", _PesoLib_getConfiguracao(lib));
	}
	_PesoLib_libera(lib);
	FreeLibrary(hModule);
	return 0;
}
