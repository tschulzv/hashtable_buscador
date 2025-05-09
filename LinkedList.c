/*
* Proyecto #3 - 'El Buscador' - AyEDII
* Implementacion de Linked List
* Tania Schulz
*/

#include <stdio.h>
#include <stdlib.h>
#include "confirm.h"
#include "LinkedList.h"
#include "HashTable.h"


List LLCreate() {
	List l;
	l = (List)malloc(sizeof(LList));
	if (l == NULL) { return NULL; }
	l->front = NULL;
	l->end = NULL;
	return l;
}


BOOLEAN LLIsEmpty(List l) {
	return (l->front == NULL);
}

BOOLEAN LLAdd(List l, int val) {
	if (l == NULL) { return FALSE; }
	Node n = (Node)malloc(sizeof(Node));
	if (n == NULL) { return FALSE; }

	n->val = val;
	n->next = NULL;
	if (isEmpty(l)) {
		l->front = n;
		l->end = n;
	}
	l->end->next = n;
	l->end = n;
	return TRUE;
}

BOOLEAN LLGetFirst(List l, void* retval) {
	CONFIRM_RETVAL(l, FALSE);
	retval = l->front;
	return TRUE;
}

BOOLEAN LLGetLast(List l, void* retval) {
	CONFIRM_RETVAL(l, FALSE);
	retval = l->end;
	return TRUE;
}

/*
void* QDequeue(Queue q, void* retval) {
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
*/

