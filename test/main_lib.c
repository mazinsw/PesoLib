#include <stdlib.h>
#include <PesoLib.h>

int main(int argc, char *argv[])
{
	PesoLib_inicializa();
	PesoLib * lib = PesoLib_cria(NULL);
	PesoLib_libera(lib);
	PesoLib_finaliza();
	return 0;
}
