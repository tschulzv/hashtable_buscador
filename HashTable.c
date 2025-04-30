/*
* Proyecto #3 - 'El Buscador' - AyEDII
* Implementacion de HashTable
* Tania Schulz
*/

#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "confirm.h"
#include "HashTable.h"

#define INITIAL_CAP 50
#define MAX_COLLISIONS 5
#define DELETED_KEY "__deleted__" // marcar las celdas borradas, para evitar problemas con el hashing doble 


/*
typedef struct _celda {
	void* valor;
	char* clave;
}Celda;

typedef struct __HashTable{
	Celda** arr;  
	int tam;
	int cap;
}_HashTable;

typedef _HashTable* HashTable;

*/

/* Crea un HashTable, devuelve el puntero a la estructura creada*/
HashTable HTCreate() {
	// crear la estructura y verificar asignacion de memoria
	HashTable ht = (HashTable)malloc(sizeof(_HashTable));
	CONFIRM_RETVAL(ht, NULL);

	// iniciar con cantidad de celdas = CAP
	// crear array y verificar asignacion de memoria 
	Celda** arr = (Celda**)calloc(INITIAL_CAP, sizeof(Celda*));
	CONFIRM_RETVAL(arr, NULL);

	// inicializar y retornar
	ht->arr = arr;
	ht->cap = INITIAL_CAP;
	ht->tam = 0;
	return ht;
}

/**
funcion privada de ayuda
Devuelve un numero entero grande (long) correspondiente a la clave pasada
por ejemplo pueden utilizar:
_stringLong("ABC") = 11*27^2 + 12*27^1 + 13*27^0 = 8356
_stringLong("09A") = 1*27^2  + 10*27^1 +  11*27^0 = 1010

o sino algo como
_stringLong("ABC") = 65*10000 + 66*100 + 67 = 656667

*/
long _stringLong(char* clave) {
	/*AYUDA SOBRE STRINGS
	Tabla de conversion de caracteres validos
	caracteres       valor ascii
	A-Z ... a-z   =  65 al 122
	0-9           =  48 al 59

	Ejemplos
	para saber el valor ascii de un caracter
	char* a = "ABC";
	int x = a[0] // a = 65

	para saber el tamanho del string
	int tam = strlen(a); // tam = 3

	AYUDA PARA CALCULOS MATEMATICOS
	int potencia = pow(27,2) // potencia = 27^2

	*/
	
	int length = strlen(clave);
	int result = 0;

	// recorrer la clave char a char, obtener el valor de cada uno y sumar
	// ejemplo _stringLong("ABC") = 65*10000 + 66*100 + 67 = 656667
	for (int i = 0; i < length; i++) {
		result += clave[i] * pow(100, length - i - 1);
	}

	return result;
}

/*
* función auxiliar para convertir los caracteres válidos
* para evitar valores demasiado grandes que se generarian al usar el ascii del char

int charToVal(char c) {
	// numeros: '0' = 1, ..., '9' = 10
	if (c >= '0' && c <= '9') return c - '0' + 1; 
	// mayusculas: 'A' = 11, ..., 'Z' = 36
	if (c >= 'A' && c <= 'Z') return c - 'A' + 11; 
	// minusculas: 'a' = 11, ..., 'z' = 36
	if (c >= 'a' && c <= 'z') return c - 'a' + 11; 

	return 0; //  caracteres invalidos
}
*/

/*
funcion privada de ayuda
devuelve el hash para cada intento de insercion/busqueda
sea i el numero de intento y clave la clave usada.
aplicar la formula:
hash(x,i) = ((stringInt(x) mod CAP) + i^2) mod CAP

**nota:** agregue el parametro cap porque el calculo debe hacerse en funcion al 
cap actual de la HT, y no del cap inicial 
*/

int _hash(char* clave, int cap, int i) {
	return ((_stringLong(clave) % cap) + i * i) % cap;
}

/* Agrega el valor con la clave dada en el hash table, en el caso de repetir la clave se sobreescriben los datos
Devuelve TRUE si tuvo exito, sino FALSE*/
BOOLEAN HTPut(HashTable p, char* clave, void* valor) {
	CONFIRM_RETVAL(p, FALSE);
	CONFIRM_RETVAL(clave, FALSE);

	int i = 0; // intentos
	while (i <= MAX_COLLISIONS) {
		// obtenemos una posicion con la funcion hash
		int pos = _hash(clave, p->cap, i);
		
		// si ya hay una clave guardada en esa celda
		if (p->arr[pos] != NULL && p->arr[pos]->clave != NULL) {
			// si es una clave repetida, sobreescribir el valor
			if (strcmp(p->arr[pos]->clave, clave) == 0) {
				p->arr[pos]->valor = valor;
				return TRUE;
			}
			// si es una clave deleted, puede sobreescribirse
			else if (strcmp(p->arr[pos]->clave, DELETED_KEY) == 0) {
				p->arr[pos]->clave == clave;
				p->arr[pos]->valor = valor;
				return TRUE;
			}
			else { // si es otra clave, es una colision
				i++; // aumentamos el num de intentos
			}
		}
		else { // si no, crear una celda y asignarle los valores
			Celda *c = (Celda*)malloc(sizeof(Celda));
			CONFIRM_RETVAL(c, FALSE);

			c->clave = strdup(clave);
			c->valor = valor;
			p->arr[pos] = c;
			return TRUE;
		}
	}
	// falta hacer el resize!!! 
	return FALSE;
}

/* Obtiene el valor asociado a la clave dentro del HashTable y lo pasa por referencia a retval
Devuelve TRUE si tuvo exito, sino FALSE*/
BOOLEAN HTGet(HashTable p, char* clave, void** retval) {
	CONFIRM_RETVAL(p, FALSE);
	CONFIRM_RETVAL(clave, FALSE);
	int i = 0; // intentos
	while (i <= MAX_COLLISIONS) {
		// obtenemos una posicion con la funcion hash
		int pos = _hash(clave, p->cap, i);
		// verificar si la clave coincide con la celda en la posicion obtenida
		if (strcmp(p->arr[pos]->clave, clave) == 0) {
			*retval = p->arr[pos]->valor;
			return TRUE;
		}
		i++; // si no, aumentamos el num de intentos
	}
	return FALSE; 
}

/* Remueve el valor asociado a la clave pasada
Devuelve TRUE si tuvo exito, sino FALSE*/
BOOLEAN HTRemove(HashTable p, char* clave) {
	CONFIRM_RETVAL(p, FALSE);
	CONFIRM_RETVAL(clave, FALSE);
	int i = 0;
	while (i <= MAX_COLLISIONS) {
		int pos = _hash(clave, p->cap, i);
		if (p->arr[pos] != NULL && p->arr[pos]->clave != NULL &&
			strcmp(p->arr[pos]->clave, clave) == 0) {
			// marcar como eliminada
			free(p->arr[pos]->clave); 
			p->arr[pos]->clave = strdup(DELETED_KEY);
			p->arr[pos]->valor = NULL; 
			p->tam--;
			return TRUE;
		}
		i++;
	}
	return FALSE;
}

/* Devuelve TRUE si el HashTable contiene la clave*/
BOOLEAN HTContains(HashTable p, char* clave) {
	CONFIRM_RETVAL(p, FALSE);
	CONFIRM_RETVAL(clave, FALSE);
	int i = 0; // intentos
	while (i <= MAX_COLLISIONS) {
		// obtenemos una posicion con la funcion hash
		int pos = _hash(clave, p->cap, i);

		// si hay un valor, verificar si su clave coincide (y que no sea una celda deleted)
		if (p->arr[pos]->clave != NULL && strcmp(p->arr[pos]->clave, clave) == 0 &&
			strcmp(p->arr[pos]->clave, DELETED_KEY) != 0) {
			return TRUE;
		}
		i++; // si no, aumentamos el num de intentos
	}
	return FALSE; // no se encontro
}

// ---------------------------------------------------------------------------
/* Devuelve la cantidad de elementos (tamanho) cargados en el HashTable*/
// ??? no debería devolver un int? 
BOOLEAN HTSize(HashTable p) {
	/*AGREGUE SU CODIGO AQUI*/
	return FALSE;
}
// ---------------------------------------------------------------------------

/* Destruye la estructura*/
BOOLEAN HTDestroy(HashTable p) {
	CONFIRM_RETVAL(p, FALSE);
	for (int i = 0; i < p->cap; i++) {
		if (p->arr[i] != NULL) {
			free(p->arr[i]->clave);
			free(p->arr[i]);
		}
	}
	free(p->arr);
	free(p);
	return TRUE;
}