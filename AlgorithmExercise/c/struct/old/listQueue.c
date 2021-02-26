#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct queue
{
	int count;
	struct node
	{
		void* data;
		struct node* link;
	}* head;
} Queue;

Queue* queueNew()
{
	Queue* queue=(Queue*)malloc(sizeof(Queue));
	queue->count=0;
	queue->head=NULL;
	return queue;
}

int queueEmpty(Queue* queue)
{
	assert(queue);
	return queue->count==0;
}

void queuePut(Queue* queue,void* data)
{
	struct node* list=(struct node*)malloc(sizeof(struct node));
	assert(list);
	assert(queue);
	list->data=data;
	list->link=queue->head;
	queue->head=list;
	++queue->count;
}
void* queueGet(Queue* queue)
{
	assert(queue);
	assert(queue->count>0);
	struct node* p=queue->head;
	struct node* u=p;
	int n=0;
	while(p)
	{
		++n;
		p=p->link;
	}
	for(int i=0;i<n-2;++i)
		u=u->link;
	p=u->link;
	void* data=p->data;
	--queue->count;
	free(p);
	u->link=NULL;
	return data;
}
void queueFree(Queue** queue)
{
	struct node* p,*u;
	for(p=(*queue)->head;p;p=u)
	{
		u=p->link;
		free(p);
	}
	free(*queue);
}
void print(Queue* queue)
{
	for(struct node* p=queue->head;p;p=p->link)
		printf("%s ",(char*)p->data);
	printf("\n");
}

int main()
{
	Queue* q=queueNew();
	int n=queueEmpty(q);
	printf("%d\n",n);
	queuePut(q,"我");
	queuePut(q,"没");
	queuePut(q,"有人啊");
	print(q);
	n=queueEmpty(q);
	printf("%d\n",n);
	queueGet(q);
	queueGet(q);
	print(q);
	queueFree(&q);
}
