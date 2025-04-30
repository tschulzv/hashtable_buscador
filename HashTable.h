#ifndef DEFINE_HASHTABLE_H
#define DEFINE_HASHTABLE_H
#include "boolean.h"
/*
 Copyright (c) 2017. Universidad Nacional de Itapua.
 
 En general no deberias tener que modificar este archivo a menos que el 
 profesor se haya equivocado!
 
*/
/*Implementacion de una tabla hash con arreglo dinamico*/
typedef struct _celda {
	void* valor;
	char* clave;
}Celda;

typedef struct __HashTable{
	Celda** arr; /*arreglo de void*  */
	int tam;
	int cap;
}_HashTable;

typedef _HashTable* HashTable;

/* Crea un HashTable, devuelve el puntero a la estructura creada*/
HashTable HTCreate();

/* Agrega el valor con la clave dada en el hash table, en el caso de repetir la clave se sobreescriben los datos
   Devuelve TRUE si tuvo exito, sino FALSE*/
BOOLEAN HTPut(HashTable p, char* clave, void* valor);

/* Obtiene el valor asociado a la clave dentro del HashTable y lo pasa por referencia a retval
   Devuelve TRUE si tuvo exito, sino FALSE*/
BOOLEAN HTGet(HashTable p, char* clave, void** retval);

/* Remueve el valor asociado a la clave pasada
Devuelve TRUE si tuvo exito, sino FALSE*/
BOOLEAN HTRemove(HashTable p, char* clave);

/* Devuelve TRUE si el HashTable contiene la clave*/
BOOLEAN HTContains(HashTable p, char* clave);

/* Devuelve la cantidad de elementos (tamanho) cargados en el HashTable*/
// no debería ser un int ???? ---------------------------------------------
BOOLEAN HTSize(HashTable p);

/* Destruye la estructura*/
BOOLEAN HTDestroy(HashTable p);

#endif