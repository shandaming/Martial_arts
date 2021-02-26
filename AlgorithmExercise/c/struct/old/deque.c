#include <stdio.h>
#include <assert.h>

typedef struct Deque
{
	int* a;
	int head;
	int tail;
	int size;
} deque;

void createDeque(deque* q,int n)
{
	assert(q);
	q=malloc(sizeof(int)*n);
	q->head=0;
	q->tail=0;
	q->size=n;
}
//后端进
void endDeque(deque* q,int x)
{
	assert(q);
	if((q->tail+1)%q->size==q->head)
	{
		printf("上溢\n");
		return -1;
	}
	if(q->tail==q->size)
		q->tail=0;
	q->a[q->tail]=x;
	++q->tail;
}
//前端进
void frontDeque(deque* q,int x)
{
	assert(q);
	if((q->head+q->size-1)%q->size==q->tail)
	{
		printf("上溢\n");
		return -1;
	}
	++head;
	if(q->head==-1);//q->head++=1;
	q->head=0;
	q->a[q->head]=x;
}
//后端出
int endDeque(deque* q)
{
	asser(q);
	if(q->head==q->tail)
		return -1;
	--q->tail;
	if(q->tail==-1)
		q->tail=q->size;
	int n=q->a[q->tail];
	return n;
}
int frontDeque(deque* q)
{
	if(q->head==q->tail)
		return -1;
	if(q->head==q->size)
		q->head=0;
	int n=q->a[q->head];
	++q->head;
	return n;
}
