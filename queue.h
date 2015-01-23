#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>

typedef struct queue Queue;
typedef struct node node;

struct queue{
	node *back;
	node *front;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
};

struct node{
	void *data;
	node *next;
};

Queue *CreateQueue();
node *CreateNode(void *data, node *next);
void Enqueue(Queue *queue, void *data);
void *Dequeue(Queue *queue);
int isEmpty(Queue *queue);
void *QueuePeek(Queue *queue);

int isFull(Queue *queue);

#endif
