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
#define MAX_RESULTS 100 // max resultados buscados
#define MAX_TOKENS // no se puede buscar mas de 10 palabras
#define TEST "test.txt"

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

typedef struct {
	int palabra_id;   // indice de la palabra en el array de tokens
	int pos;          // posicion actual de la lista de posiciones
	Node nodo;       // puntero al nodo actual de la lista 
} HeapItem;


// funcion usada para heapsort de PrioValue
// con posicion en el libro como criterio de ordenacion
int comparator(const void* a, const void* b) {
	const HeapItem* x = (const HeapItem*)a;
	const HeapItem* y = (const HeapItem*)b;
	
	if (x->pos < y->pos) return -1;
	if (x->pos > y->pos) return 1;
	return 0;
}

// buscar palabras
// utiliza un metodo de 'ventana deslizante' que compara el menor de los indices 
// con el mayor, y luego sigue comparando con los siguientes al menor 
int search_words(HashTable ht, char* tokens[], int words_num, int results[]) {
	if (words_num == 0) return 0; // no se busco ninguna palabra

	// crear un array de HeapItems que usaremos para ordenar los indices
	HeapItem* heap = malloc(words_num * sizeof(HeapItem));

	// inicializar nodos y heap
	for (int i = 0; i < words_num; i++) {
		// si  una de las palabrasno existe en la hash table finaliza
		if (!HTContains(ht, tokens[i])) return 0;

		// obtener la lista de ubicaciones de esa palabra
		List lista; 
		HTGet(ht, tokens[i], (void**)&lista);
		CONFIRM_RETVAL(lista, -1);

		heap[i].palabra_id = i; // guardar su indice en la lista de tokens
		heap[i].nodo = lista->front; // iniciar en el frente de la lista
		heap[i].pos = lista->front->val;
	}

	int encontrados = 0; // conteo de matches

	while (1) {
		
		// usar la funcion qsort para ordenar los indices y obtener el minimo y maximo
		qsort(heap, words_num, sizeof(HeapItem), comparator);

		int min_pos = heap[0].pos; // el indice mas pequenho
		int max_pos = heap[words_num - 1].pos; // el mas grande

		// si el mas pequenho y el mas grande estan a una distancia de 100 caracteres
		// los demas tambien deben estarlo -> MATCH
		if (max_pos - min_pos <= 100) {
			if (encontrados < MAX_RESULTS) {
				results[encontrados++] = min_pos;
			}
		}

		// avanzar la ventana deslizante (lista donde estaba el minimo)
		//int id = heap[0].palabra_id; 
		Node next = heap[0].nodo->next;

		if (!next) break; // ya no hay elementos en esa lista
		
		heap[0].nodo = next; // avanzar al siguiente nodo
		heap[0].pos = next->val + min_pos; // ubicacion en el libro del siguiente nodo, sumado pq es un delta
	}

	return encontrados; // retorna cantidad de matches
}

// FALTA GUARDAR EL OFFSET DE LA LINEA Y LINE_NUM
BOOLEAN IndexFile(HashTable ht, const char* filename, int** line_start, int* line_num) {
	FILE* f = fopen(filename, "r");
	if (!f) {
		printf("No se pudo abrir el archivo.\n");
		return FALSE;
	}

	char line[LINE_MAX]; // guarda la linea leida
	long fileIndex  = 0; // indice de caracteres leidos 

	while (fgets(line, LINE_MAX, f)) {
		int lineLen = strlen(line); // largo de la linea
		int lineIndex = 0; // indice dentro de la linea

		// recorrer la linea
		while (lineIndex < lineLen) {
			// si comienza con un signo de puntuacion, seguir avanzando
			if (!isalpha(line[lineIndex])) lineIndex++;
			int wordStart = lineIndex; //  inicio de la palabra

			// seguir avanzando hasta encontrar otro signo de puntuacion
			while (lineIndex < lineLen && isalpha(line[lineIndex])) lineIndex++;
			int wordEnd = lineIndex; // llegamos al fin
			int wordLen = wordEnd - wordStart;

			// ignorar palabras de menos de 3 chars
			if (wordLen > 3) {
				char word[50];
				// copiar desde el indice donde inicia y poner null al final
				strncpy(word, (line + wordStart), wordLen);
				word[wordLen] = '\0';

				// FALTA CONVERTIR A MINUSCULAS !!!

				List l;
				HTGet(ht, word, (void**)l);
				int absPos = lineIndex + wordStart;

				// verificar si ya existe la palabra en la HT
				if (l == NULL) {
					// si no existe, crear nueva lista para la palabra
					l = LLCreate();
					CONFIRM_RETVAL(l, FALSE);

					// agregar indice a la lista
					if (!LLAdd(l, absPos)) return FALSE;
					// luego agregar a HashTable
					HTPut(ht, word, l);
				}
				else {
					// si ya existe, guardamos la posicion como un delta
					int posDelta = absPos - (l->end->val);
					//agregar a la lista existente
					LLAdd(l, posDelta);
					// sobreescribir en la HashTable
					HTPut(ht, word, l);
				}

			}
		}
		fileIndex += lineLen; // avanzar el indice del archivo
	}
	return TRUE;
}

/*programa de prueba, implemente su codigo del proyecto 3 aqui*/

int menu() {
	printf(" --- BUSCADOR DE LIBROS ---");
	printf("\nSeleccione una opcion: \n1. Don Quijote \n 2. La isla del tesoro \n4. Todos");

}

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

