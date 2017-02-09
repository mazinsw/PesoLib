#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
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

#ifdef _WIN64
#define DLL_FILE "../bin/x64/PesoLib.dll"
#else
#define DLL_FILE "../bin/x86/PesoLib.dll"
#endif

PesoLib_criaFunc _PesoLib_cria;
PesoLib_getUltimoPesoFunc _PesoLib_getUltimoPeso;
PesoLib_solicitaPesoFunc _PesoLib_solicitaPeso;
PesoLib_aguardaEventoFunc _PesoLib_aguardaEvento;
PesoLib_setConfiguracaoFunc _PesoLib_setConfiguracao;
PesoLib_getConfiguracaoFunc _PesoLib_getConfiguracao;
PesoLib_cancelaFunc _PesoLib_cancela;
PesoLib_liberaFunc _PesoLib_libera;


static DWORD WINAPI inputListener(LPVOID param)
{
	PesoLib* lib = (PesoLib*)param;
	int done = 0;
	float preco = 5.00f;
	char config[100], input[100];
	
	while(!done)
	{
		int ch = getch();
		switch(ch)
		{
		case '\r':
		case '\n':
			_PesoLib_solicitaPeso(lib, 0.00f);
			break;
		case '\b':
			system("cls");
			break;
		case '$':
			printf("Preco = %.2f\n", preco);
			_PesoLib_solicitaPeso(lib, preco);
			break;
		case '\'':
			// port:COM2;baund:9600;data:8;stop:1;parity:none;flow:none;timeout:1000;retry:1500;alive:10000;
			config[0] = '\0';
			input[0] = '\0';
			printf("Digite a porta: ");
			scanf("%99[^\n]", input);
			scanf("%*1[\n]");
			if(input[0] != '\0')
			{
				strcat(config, "port:");
				strcat(config, input);
				strcat(config, ";");
			}
			input[0] = '\0';
			printf("Digite a velocidade: ");
			scanf("%99[^\n]", input);
			scanf("%*1[\n]");
			if(input[0] != '\0')
			{
				strcat(config, "baund:");
				strcat(config, input);
				strcat(config, ";");
			}
			input[0] = '\0';
			printf("Digite os bits de dados: ");
			scanf("%99[^\n]", input);
			scanf("%*1[\n]");
			if(input[0] != '\0')
			{
				strcat(config, "data:");
				strcat(config, input);
				strcat(config, ";");
			}
			input[0] = '\0';
			printf("Digite os bits de parada (1.5, 2): ");
			scanf("%99[^\n]", input);
			scanf("%*1[\n]");
			if(input[0] != '\0')
			{
				strcat(config, "stop:");
				strcat(config, input);
				strcat(config, ";");
			}
			input[0] = '\0';
			printf("Digite a paridade (none, space, mark, even, odd): ");
			scanf("%99[^\n]", input);
			scanf("%*1[\n]");
			if(input[0] != '\0')
			{
				strcat(config, "parity:");
				strcat(config, input);
				strcat(config, ";");
			}
			input[0] = '\0';
			printf("Digite o fluxo (dsrdtr, rtscts, xonxoff): ");
			scanf("%99[^\n]", input);
			scanf("%*1[\n]");
			if(input[0] != '\0')
			{
				strcat(config, "flow:");
				strcat(config, input);
				strcat(config, ";");
			}
			input[0] = '\0';
			printf("Digite o tempo de espera: ");
			scanf("%99[^\n]", input);
			scanf("%*1[\n]");
			if(input[0] != '\0')
			{
				strcat(config, "timeout:");
				strcat(config, input);
				strcat(config, ";");
			}
			input[0] = '\0';
			printf("Digite o tempo de reconexao: ");
			scanf("%99[^\n]", input);
			scanf("%*1[\n]");
			if(input[0] != '\0')
			{
				strcat(config, "retry:");
				strcat(config, input);
				strcat(config, ";");
			}
			input[0] = '\0';
			printf("Digite o tempo de vida da conexao: ");
			scanf("%99[^\n]", input);
			scanf("%*1[\n]");
			if(input[0] != '\0')
			{
				strcat(config, "alive:");
				strcat(config, input);
				strcat(config, ";");
			}
			_PesoLib_setConfiguracao(lib, config);
			printf("OK\n");
			break;
		case '?':
			printf("Comandos:\n\n");
			printf("\tSolicitar peso: Enter\n");
			printf("\tAlterar preco: $\n");
			printf("\tLimpar tela: Backspace\n");
			printf("\tMostrar configuracao: C\n");
			printf("\tAlterar configuracao: '\n");
			printf("\tSair: Q\n\n");
			break;
		case 'c':
		case 'C':
			printf("Configuracao:\n%s\n\n", _PesoLib_getConfiguracao(lib));
			break;
		case 'q':
		case 'Q':
			done = 1;
			_PesoLib_cancela(lib);
			break;
		}
	}
	return 0;
}

int main(int argc, char *argv[])
{
	DWORD startTick = GetTickCount();
	HMODULE hModule = LoadLibrary(DLL_FILE);
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
	PesoLib* lib = _PesoLib_cria(argc > 0?argv[1]:NULL);
	int evento;
	DWORD endTick;
	HANDLE hThread = CreateThread(NULL, 0, inputListener,
						   (void*)lib, 0, NULL);
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
	}
	_PesoLib_libera(lib);
	FreeLibrary(hModule);
	WaitForSingleObject(hThread, INFINITE);
	return 0;
}
