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
#define MAX_TOKENS 10 // no se puede buscar mas de 10 palabras
#define TEST "test.txt"

// separadores de palabras (espacios, signos de puntuacion etc)
unsigned char separators[256] = { 0 };

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


// funcion usada para heapsort de HeapItem
// con posicion en el libro como criterio de ordenacion
int comparator(const void* a, const void* b) {
	const HeapItem* x = (const HeapItem*)a;
	const HeapItem* y = (const HeapItem*)b;

	if (x->pos < y->pos) return -1;
	if (x->pos > y->pos) return 1;
	//return 0;
}

// verificar si el caracter es un separador
int is_separator(unsigned char c) {
	return separators[c];
}

// obtener linea segun la posicion 
int get_line(int* line_offsets, int num_lineas, int pos) {
	int lo = 0, hi = num_lineas - 1;
	while (lo <= hi) {
		int mid = (lo + hi) / 2;
		if (line_offsets[mid] <= pos) {
			if (mid == num_lineas - 1 || line_offsets[mid + 1] > pos) {
				return mid;
			}
			lo = mid + 1;
		}
		else {
			hi = mid - 1;
		}
	}
	return -1; // no encontrado
}


// obtener el contexto de la posicion (linea, linea anterior y posterior)
void get_context(FILE* archivo, int* line_offsets, int num_lines, int pos) {
	// Buscar la línea donde está la posición
	int linea = get_line(line_offsets, num_lines, pos);

	if (linea == -1) {
		printf("No se pudo determinar la linea.\n");
		return;
	}

	printf("\n--- Coincidencia encontrada ---\n");

	char buffer[LINE_MAX];
	for (int i = linea - 1; i <= linea + 1; i++) {
		if (i >= 0 && i < num_lines) {
			fseek(archivo, line_offsets[i], SEEK_SET);
			if (fgets(buffer, LINE_MAX, archivo)) {
				printf("Linea %d: %s", i + 1, buffer);
			}
		}
	}
	printf("\n");
}


int search_words2(FILE* f, HashTable ht, char* tokens[], int words_num, int results[], int* line_offsets, int num_lineas) {
	if (words_num == 0) return 0;

	HeapItem* heap = malloc(words_num * sizeof(HeapItem));
	CONFIRM_RETVAL(heap, -1);

	int acumulados[MAX_TOKENS]; // acumuladores de posición absoluta

	for (int i = 0; i < words_num; i++) {
		if (!HTContains(ht, tokens[i])) return 0;

		List lista;
		HTGet(ht, tokens[i], (void**)&lista);
		CONFIRM_RETVAL(lista, -1);

		heap[i].palabra_id = i;
		heap[i].nodo = lista->front;
		heap[i].pos = lista->front->val;

		acumulados[i] = heap[i].pos; // inicializar acumulador
	}

	int encontrados = 0;

	while (1) {
		qsort(heap, words_num, sizeof(HeapItem), comparator);

		int min_pos = heap[0].pos;
		int max_pos = heap[words_num - 1].pos;

		if (max_pos - min_pos <= 100) {
			if (encontrados < MAX_RESULTS) {
				results[encontrados++] = min_pos;
				get_context(f, line_offsets, num_lineas, min_pos);
			}
		}

		// avanzar la lista correspondiente al mínimo
		int id = heap[0].palabra_id;
		Node next = heap[0].nodo->next;
		if (!next) break;

		acumulados[id] += next->val;         // actualizar acumulador
		heap[0].nodo = next;
		heap[0].pos = acumulados[id];        // actualizar posición absoluta en el heap
	}

	free(heap);
	return encontrados;
}


// buscar palabras
// utiliza un metodo de 'ventana deslizante' que compara el menor de los indices 
// con el mayor, y luego sigue comparando con los siguientes al menor 
int search_words(FILE* f, HashTable ht, char* tokens[], int words_num, int results[], int* line_offsets, int num_lineas) {
	if (words_num == 0) return 0; // no se busco ninguna palabra

	// crear un array de HeapItems que usaremos para ordenar los indices
	HeapItem* heap = malloc(words_num * sizeof(HeapItem));
	CONFIRM_RETVAL(heap, -1);

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

				get_context(f, line_offsets, num_lineas, min_pos);
			}
		}

		// avanzar la ventana deslizante (lista donde estaba el minimo)
		//int id = heap[0].palabra_id; 

		Node next = heap[0].nodo->next;
		if (!next) break; // ya no hay elementos en esa lista
		
		heap[0].pos += next->val; // ubicacion en el libro del siguiente nodo, sumado pq es un delta
		heap[0].nodo = next; // avanzar al siguiente nodo
		//heap[0].pos = next->val + min_pos; 
	}

	return encontrados; // retorna cantidad de matches
}


BOOLEAN IndexFile1(FILE* f, HashTable ht, const char* filename, int** line_offsets, int* line_num) {
	char line[LINE_MAX]; // guarda la linea leida
	long fileIndex  = 0; // indice de caracteres leidos 
	int numLines = 0; // conteo de lineas del archivo
	int capLines = 1024;

	// array de offsets de lineas
	int* offsets = malloc(sizeof(int) * capLines);
	CONFIRM_RETVAL(offsets, FALSE);

	while (fgets(line, LINE_MAX, f)) {
		int lineLen = strlen(line); // largo de la linea
		int lineIndex = 0; // indice dentro de la linea

		if (numLines == capLines) {
			capLines *= 2;
			offsets = realloc(offsets, sizeof(int) * capLines);
			if (!offsets) return FALSE;
		}
		// guardar el offset de la linea
		offsets[numLines] = (int)ftell(f) - strlen(line);
		numLines++;

		// recorrer la linea
		while (lineIndex < lineLen) {
			// si comienza con un signos de puntuacion, seguir avanzando
			while (lineIndex < lineLen && is_separator((unsigned char)line[lineIndex]))
				lineIndex++;
			int wordStart = lineIndex; //  inicio de la palabra

			// seguir avanzando hasta encontrar otro signo de puntuacion
			//while (lineIndex < lineLen && !is_separator(line[lineIndex])) lineIndex++;
			while (lineIndex < lineLen && !is_separator((unsigned char)line[lineIndex]))
				lineIndex++;
			int wordEnd = lineIndex; // llegamos al fin de la palabra
			int wordLen = wordEnd - wordStart;

			// ignorar palabras de menos de 3 chars
			if (wordLen >= 3) {
				char word[64];
				// copiar desde el indice donde inicia y poner null al final
				if (wordLen > sizeof(word)) wordLen = sizeof(word) - 1;
				strncpy(word, (line + wordStart), wordLen);
				word[wordLen] = '\0';

				// FALTA CONVERTIR A MINUSCULAS !!!


				long line_offset = ftell(f) - strlen(line);
				int absPos = (int)(line_offset + wordStart);

				List l = NULL;
				if (!HTGet(ht, word, (void**)&l) || l == NULL) {
					// No existe: crear y agregar
					l = LLCreate();
					CONFIRM_RETVAL(l, FALSE);

					// agregar indice a la lista
					if (!LLAdd(l, absPos)) return FALSE;
					// luego agregar a HashTable
					HTPut(ht, word, l);
				}
				else {
					// Verificar que l->end existe
					if (l->end == NULL) {
						LLAdd(l, absPos);
					}
					else {
						// si ya existe, guardamos la posicion como un delta
						int delta = absPos - l->end->val;
						//agregar a la lista existente
						LLAdd(l, delta);
					}
				}

			}
		}
		fileIndex += lineLen; // avanzar el indice del archivo
	}
	*line_offsets = offsets;
	*line_num = numLines;
	return TRUE;
}

// version con ftell
BOOLEAN IndexFile2(FILE* f, HashTable ht, const char* filename, int** line_offsets, int* line_num) {
	char line[LINE_MAX]; // guarda la linea leida
	//long fileIndex  = 0; // indice de caracteres leidos 
	int numLines = 0; // conteo de lineas del archivo
	int capLines = 1024;

	// array de offsets de lineas
	int* offsets = malloc(sizeof(int) * capLines);
	CONFIRM_RETVAL(offsets, FALSE);

	while (1) {
		long offset = ftell(f);
		if (!fgets(line, LINE_MAX, f)) break;

		if (numLines == capLines) {
			capLines *= 2;
			offsets = realloc(offsets, sizeof(int) * capLines);
			if (!offsets) return FALSE;
		}

		offsets[numLines++] = (int)offset;

		int lineLen = strlen(line);
		int lineIndex = 0;

		while (lineIndex < lineLen) {
			while (lineIndex < lineLen && is_separator((unsigned char)line[lineIndex]))
				lineIndex++;
			int wordStart = lineIndex;

			while (lineIndex < lineLen && !is_separator((unsigned char)line[lineIndex]))
				lineIndex++;
			int wordLen = lineIndex - wordStart;

			if (wordLen > 3) {
				char word[64];
				if (wordLen >= sizeof(word)) wordLen = sizeof(word) - 1;
				strncpy(word, &line[wordStart], wordLen);
				word[wordLen] = '\0';

				int absPos = (int)(offset + wordStart);
				//int absPos = (int)(ftell(f) - strlen(line) + wordStart);

				List l = NULL;
				if (!HTGet(ht, word, (void**)&l) || l == NULL) {
					l = LLCreate();
					CONFIRM_RETVAL(l, FALSE);
					LLAdd(l, absPos);
					HTPut(ht, word, l);
				}
				else {
					int delta = absPos - l->end->val;
					LLAdd(l, delta);
				}
			}
		}
	}

	*line_offsets = offsets;
	*line_num = numLines;
	return TRUE;
}


/*programa de prueba, implemente su codigo del proyecto 3 aqui*/

void searchBook(char* filename) {
	// crear la ht
	HashTable ht = HTCreate();
	CONFIRM_RETVAL(ht, 1);
	// array de int para guardar donde comienza cada linea
	int* line_offsets;
	int line_num = 0;

	// abrir el archivo
	FILE* f = fopen(TEST, "r");
	if (!f) {
		printf("No se pudo abrir el archivo.\n");
		return 1;
	}

	// indexar el archivo
	if (!IndexFile2(f, ht, filename, &line_offsets, &line_num)) {
		printf("No se pudo indexar el archivo");
		return 1;
	}
	printf("Se indexo el archivo");
	// para poder leer el archivo desde el inicio (mostrar pedazos de texto)
	rewind(f);

	// entrada de usuario
	char entrada[256];
	while (1) {
		printf("\n> Buscar palabras (o 'salir'): ");
		if (!fgets(entrada, sizeof(entrada), stdin)) break;
		entrada[strcspn(entrada, "\n")] = 0;

		if (strcmp(entrada, "salir") == 0) return 0;

		// dividir en tokens con un espacio como delimitador
		char* tokens[10];
		int words_num = 0;
		char* tok = strtok(entrada, " ");
		while (tok && words_num < MAX_TOKENS) {
			if (strlen(tok) > 3)
				tokens[words_num++] = tok;
			tok = strtok(NULL, " ");
		}

		if (words_num == 0) {
			printf("Error: No hay palabras válidas.\n");
			continue;
		}

		int resultados[MAX_RESULTS];
		int resultNum = search_words(f, ht, tokens, words_num, resultados, line_offsets, line_num);

		if (resultNum == 0) {
			printf("No se encontraron resultados.\n");
		}

	}
	fclose(f);
	free(line_offsets);
	HTDestroy(ht);

}


int main(int argc, char** argv) {
	
	// inicializar los separadores
	const char* set = " \n\r\t.,;:¡!?\"'()[]{}-_";
	for (int i = 0; set[i]; i++) {
		separators[(unsigned char)set[i]] = 1;
	}

	// array de rutas de libros 
	char* libros = ["DonQuijote.txt", "la_isla_del_tesoro.txt",
	"tesoro.txt"
	"lobo.txt"];
	int option;

	// imprimir el menu
	printf(" --- EL BUSCADOR ---");
	printf("\nSeleccione un libro: \n1. Don Quijote \n2. La isla del tesoro \n3. El tesoro misterioso \n4. Romance de lobos \n5. Buscar en todos");
	printf("\nIngrese el numero correspondiente: ");

	scanf("%d", &option);

	
	// crear la ht
	HashTable ht = HTCreate();
	CONFIRM_RETVAL(ht, 1);
	// array de int para guardar donde comienza cada linea
	int* line_offsets; 
	int line_num = 0;

	// abrir el archivo
	FILE* f = fopen(TEST, "r");
	if (!f) {
		printf("No se pudo abrir el archivo.\n");
		return 1;
	}

	// indexar el archivo
	if(!IndexFile2(f, ht, TEST, &line_offsets, &line_num)) {
		printf("No se pudo indexar el archivo");
		return 1;
	}
	printf("Se indexo el archivo");
	// para poder leer el archivo desde el inicio (mostrar pedazos de texto)
	rewind(f);

	// entrada de usuario
	char entrada[256];
	while (1) {
		printf("\n> Buscar palabras (o 'salir'): ");
		if (!fgets(entrada, sizeof(entrada), stdin)) break;
		entrada[strcspn(entrada, "\n")] = 0;

		if (strcmp(entrada, "salir") == 0) return 0;

		// dividir en tokens con un espacio como delimitador
		char* tokens[10];
		int words_num = 0;
		char* tok = strtok(entrada, " ");
		while (tok && words_num < MAX_TOKENS) {
			if (strlen(tok) > 3)
				tokens[words_num++] = tok;
			tok = strtok(NULL, " ");
		}

		if (words_num == 0) {
			printf("Error: No hay palabras válidas.\n");
			continue;
		}

		int resultados[MAX_RESULTS];
		int resultNum = search_words(f, ht, tokens, words_num, resultados, line_offsets, line_num);

		if (resultNum == 0) {
			printf("No se encontraron resultados.\n");
		}
		
	}
	fclose(f);
	free(line_offsets);
	HTDestroy(ht);
	system("PAUSE");

	return (EXIT_SUCCESS);
}

