#include "boolean.h"

typedef struct __Node {
	void* val;
	struct __Node* next;
}_Node;

typedef _Node* Node;

// optimizacion de LinkedList
// referencia al ult elemento para insercion al final
typedef struct _LList {
	Node front;
	Node end;
} LList;

typedef LList* List;

/*crea una linked list*/
List LLCreate();

/*verificar si linked list esta vacia*/
BOOLEAN LLIsEmpty(List l);

/*inserta al final de la lista*/
BOOLEAN LLAdd(List l, void* x);

/*obtener primer elemento de la lista*/
BOOLEAN LLGetFirst(List l, void* retval);

BOOLEAN LLGetLast(List l, void* retval);


//void* LLRemove(Queue q, void* retval);
