/*
* Proyecto #3 - 'El Buscador' - AyEDII
* Implementacion de Queue
* Tania Schulz
*/

#include <stdio.h>
#include <stdlib.h>
#include "confirm.h"
#include "Queue.h"


Queue create() {
	Queue q;
	q = (Queue)malloc(sizeof(LLQueue));
	if (q == NULL) { return NULL; }
	q->front = NULL;
	q->end = NULL;
	return q;
}


BOOLEAN isEmpty(Queue q) {
	return (q->front == NULL && q->end == NULL);
}

BOOLEAN enqueue(Queue q, void* x) {
	if (q == NULL) { return FALSE; }
	Node n = (Node)malloc(sizeof(Node));
	if (n == NULL) { return FALSE; }

	n->val = x;
	n->next = NULL;
	if (isEmpty(q)) {
		q->front = n;
		q->end = n;
	}
	q->end->next = n;
	q->end = n;
	return TRUE;
}

void* dequeue(Queue q, void* retval) {
	if (q == NULL) { return NULL; }
	if (!isEmpty(q)) {
		Node front = q->front;
		retval = q->front->val;
		q->front = q->front->next;
		free(front);
		return;
	}
	printf("Queue is empty\n");
	return NULL;
}
