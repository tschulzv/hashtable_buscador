/*
* Proyecto #3 - 'El Buscador' - AyEDII
* Tania Schulz
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "confirm.h"
#include "HashTable.h"
/* Implementado una vez por programa para establecer como manejar errores */
extern void GlobalReportarError(char* pszFile, int  iLine) {

	/* Siempre imprime el error */
	fprintf(
		stderr,
		"\nERROR NO ESPERADO: en el archivo %s linea %u",
		pszFile,
		iLine
	);

}

/*programa de prueba, implemente su codigo del proyecto 3 aqui*/

int main(int argc, char** argv) {

	/*ejemplo de uso*/
	HashTable d = HTCreate();
	int v1 = 0;
	/*meter cualquier verdura*/
	HTPut(d, "papa", (void*)3000);
	HTPut(d, "cebolla", (void*)4000);
	HTPut(d, "perejil", (void*)500);
	/*sacar cualquier verdura*/
	HTGet(d, "papa", (void**)&v1);
	printf("papa %d", v1);
	//CONFIRM_RETURN(1 == 0);
	HTDestroy(d);
	system("PAUSE");

	return (EXIT_SUCCESS);
}

