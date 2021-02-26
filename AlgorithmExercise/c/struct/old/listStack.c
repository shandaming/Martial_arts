#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

typedef struct stack
{
	int count;
	struct elem
	{
		void* x;
		struct elem* link;
	}* head;
} Stack;

Stack* stackNew()
{
	Stack* stk=malloc(sizeof(Stack));
	stk->count=0;
	stk->head=NULL;

	return stk;
}
int stackEmpty(Stack* stk)
{
	assert(stk);
	return stk->count==0;
}
void stackPush(Stack* stk,void* x)
{
	struct elem* t=malloc(sizeof(struct elem));
	assert(stk);
	t->x=x;
	t->link=stk->head;
	stk->head=t;
	++stk->count;
}
void* stackPop(Stack* stk)
{
	void* x;
	struct elem* t;
	assert(stk);
	assert(stk->count>0);
	t=stk->head;
	stk->head=t->link;
	--stk->count;
	x=t->x;
	free(t);
	return x;
}
void stackFree(Stack* stk)
{
	struct elem* t,*u;
	assert(stk&&*stk);
	for(t=(*stk)->head;t;t=u)
	{
		u->t->link;
		free(t);
	}
	free(*stk);
}
