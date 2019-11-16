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
#include <conio.h>
#include <stdio.h>

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
			PesoLib_solicitaPeso(lib, 0.00f);
			break;
		case '\b':
			system("cls");
			break;
		case '$':
			printf("Preco = %.2f\n", preco);
			PesoLib_solicitaPeso(lib, preco);
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
			PesoLib_setConfiguracao(lib, config);
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
			printf("Configuracao:\n%s\n\n", PesoLib_getConfiguracao(lib));
			break;
		case 'q':
		case 'Q':
			done = 1;
			PesoLib_cancela(lib);
			break;
		}
	}
	return 0;
}

int main(int argc, char** argv)
{
	DWORD startTick = GetTickCount();
	int gramas;

	PesoLib_inicializa();
	PesoLib* lib = PesoLib_cria(argc > 0?argv[1]:NULL);
	int evento;
	DWORD endTick;
	HANDLE hThread = CreateThread(NULL, 0, inputListener,
						   (void*)lib, 0, NULL);
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
		case Evento_PesoInstavel:
			gramas = PesoLib_getUltimoPeso(lib);
			printf("Peso instavel: %.3f Kg\n", gramas / 1000.0);
			continue;
		}
		gramas = PesoLib_getUltimoPeso(lib);
		printf("Peso recebido: %.3f Kg\n", gramas / 1000.0);
	}
	PesoLib_libera(lib);
	PesoLib_finaliza();
	WaitForSingleObject(hThread, INFINITE);
	return 0;
}
#endif