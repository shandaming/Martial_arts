#include <stdio.h>

typedef struct Queue
{
	int* q;
	int size;
	int head;
	int tail;
} queue;

void createInit(queue* q,int size)
{
	q=malloc(sizeof(struct Queue)*size);
	q->size=size;
	q->head=0;
	q->tail=0;
}
void enqueue(queue* q,int x)
{
	if(((q->tail+1)%q->size)==q->head)
		printf("queue is full\n");
	else
	{
		q->a[a->tail]=x;
		q->tail=(q->tail+1)%q->size;
	}
}
int dequeue(queue* q)
{
	if(q->tail==q->head)
		return -1;
	else
	{
		int val=q->a[a->head];
		q->head=((q->head++)%q->size);
		return val;
	}
}
void freeQueue(queue* q)
{
	free(q);
	q=NULL;
}
