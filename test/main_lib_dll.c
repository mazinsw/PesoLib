#include <stdlib.h>
#include <PesoLib.h>

int main(int argc, char *argv[])
{
	PesoLib * lib = PesoLib_cria(NULL);
	PesoLib_libera(lib);
	return 0;
}
