#include "boolean.h"

typedef struct __Node {
	void* val;
	_Node* next;
}_Node;

typedef _Node* Node;

typedef struct _LLQueue {
	Node front;
	Node end;
} LLQueue;

typedef LLQueue* Queue;

Queue create();

BOOLEAN isEmpty(Queue q);

BOOLEAN enqueue(Queue q, void* x);

void* dequeue(Queue q, void* retval);
