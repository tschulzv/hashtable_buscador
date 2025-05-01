/*
* Proyecto #3 - 'El Buscador' - AyEDII
* Tania Schulz
*/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "confirm.h"
#include "LinkedList.h"
#include "HashTable.h"
#define LINE_MAX 2048 // max tamanho de linea

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

BOOLEAN IndexFile(HashTable ht, const char* filename) {
	FILE* f = fopen(filename, "r");
	if (!f) {
		printf("No se pudo abrir el archivo.\n");
		return FALSE;
	}

	char line[LINE_MAX]; // guarda la linea leida
	long index  = 0; // indice de caracteres leidos 

	while (fgets(line, LINE_MAX, f)) {
		int lineLen = strlen(line);
		int word_start = -1;

		for (int i = 0; i <= lineLen; i++) {
			if (isalpha(line[i])) {
				if (word_start == -1) word_start = i;
			}
			else {
				if (word_start != -1) {}

		// ignoramos palabras menores a 3 caracteres
		if (len <= 3) {
			continue;
		}

		// quitar signos de puntuacion al inicio o final de la palabra
		if (ispunct(word[len - 1])) {
			word[len - 1] = '\0';
		}
		else if (ispunct(word[0])) {
			// como quitar el signo de puntuacion al inicio?
		}

		// convertir a minusculas
		for (int i = 0; word[i]; i++) {
			word[i] = tolower(word[i]);
		}

		// verificar si ya existe la palabra en la HT
		List l = NULL;
		HTGet(ht, word, (void**)l);

		if (l == NULL) {
			// si no existe, crear nueva lista para la palabra
			l = LLCreate();
			CONFIRM_RETVAL(l, FALSE);
			// agregar indice a la lista
			if (!LLAdd(l, index)) return FALSE;
			// luego agregar a HashTable
			HTPut(ht, word, l);
		}
		else {
			//agregar a la lista existente
			LLAdd(l, index);
			// sobreescribir en la HashTable
			HTPut(ht, word, l);
		}

		return TRUE;
	}
}

/*programa de prueba, implemente su codigo del proyecto 3 aqui*/

int main(int argc, char** argv) {

	/*ejemplo de uso*/
	HashTable d = HTCreate();
	int *val = 0;
	/*meter cualquier verdura*/
	int *v1 = malloc(sizeof(int));
	*v1 = 3000;
	HTPut(d, "papa", v1);

	int* v2 = malloc(sizeof(int));
	*v2 = 4000;
	HTPut(d, "cebolla", v2);

	int* v3 = malloc(sizeof(int));
	*v3 = 500;
	HTPut(d, "perejil", v3);
	HTRemove(d, "cebolla");

	/*meter cualquier verdura*/
	int* v4 = malloc(sizeof(int));
	*v4 = 1000;
	HTPut(d, "zanahoria", v1);

	int* v5 = malloc(sizeof(int));
	*v5 = 8000;
	HTPut(d, "tomate", v2);

	int* v6 = malloc(sizeof(int));
	*v6 = 1500;
	HTPut(d, "ajo", v3);

	HTPrint(d);
	/*sacar cualquier verdura*/
	HTGet(d, "papa", (void**)&val);
	printf("papa %d\n", *val);
	//CONFIRM_RETURN(1 == 0);
	HTDestroy(d);
	free(v1);
	free(v2);
	free(v3);
	system("PAUSE");

	return (EXIT_SUCCESS);
}

