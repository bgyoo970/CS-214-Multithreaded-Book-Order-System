#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define QSIZE	16

/* this queue must initially be empty */
Queue *CreateQueue() {
	Queue *queue = (Queue*)malloc(sizeof(Queue));

	if(queue){
		queue->front = NULL;
		queue->back = NULL;
	
		/* empty the queue mutex or cond of any possible garbage */
		if (pthread_mutex_init(&queue->mutex, NULL) != 0) {
			free(queue);
			queue = NULL;
		}
		else if (pthread_cond_init(&queue->cond, NULL) != 0) {
			pthread_mutex_destroy(&queue->mutex);
			free(queue);
			queue = NULL;
		}
	}
	return queue;
}


node *CreateNode(void *data, node *next) {
	node *newNode = (node*)malloc(sizeof(node));
	newNode->data = data;
	newNode->next = next;

	return newNode;
}

void Enqueue(Queue *queue, void *data) {
	node *newNode = CreateNode(data, NULL);
	
	//Error check
	if (!queue){
		printf("Error with Enqueue. No Queue exists. \n");
		return;
	}
	
	//Base Cases
	if (queue->front == NULL) {
		queue->front = newNode;
	}
	if (queue->back == NULL) {
		queue->back = newNode;
		queue->back->next = queue->back;
	}

	else {
		newNode->next = queue->back->next;
		queue->back->next = newNode;
		queue->back = newNode;
	}

}

void *Dequeue(Queue *queue) {
	void *data;
	node *temp;
	
	// Base Case
	if (queue == NULL) { printf("Cannot dequeue, queue DNE. Returning NULL \n"); return NULL;}
	if (queue->back == NULL) { printf("Cannot dequeue, queue is empty. Returning NULL \n"); return NULL;}

	// Dequeue a single element
	if (queue->back == queue->back->next || !queue->back->next) {
		data = queue->back->data;
		free(queue->back);
		queue->back = NULL;
	}
	
	// Dequeue from multiple elements
	else {
		temp = queue->back->next;
		data = queue->back->next->data;
		queue->back->next = queue->back->next->next;
		free(temp);
	}
	return data;
}

int isEmpty(Queue *queue) {
	if (queue == NULL || queue->back == NULL)
		return 1;
	return 0;
}

void *QueuePeek(Queue *queue) {
	if (queue->back != NULL) {
		if(queue->back->next != NULL) {
			return queue->back->next->data;
		}
	}
	return NULL;
}
int isFull(Queue *queue) {
	if (queue == NULL) { printf("Error. No Queue\n"); }
	int ct = 0;
	node *temp;
	for (temp = queue->back; temp != NULL; temp = temp->next) {
		ct++;
	}
	if (ct == QSIZE)
		return 1;

	return 0;

}




