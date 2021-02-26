#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>

typedef struct list
{
	struct list* next;
	void* data;
} List;

//前插
List* listPush(List* list,void* x)
{
	List* p=malloc(sizeof(List));
	assert(p);
	p->data=x;
	p->next=list;

	return p;
}
List* listList(void* x,...)
{
	va_list ap;
	List* list;
	List** p=&list;//指向list的地址
	va_start(ap,x);
	for(;x;x=va_arg(ap,void*))
	{
		*p=malloc(sizeof(*p));
		(*p)->data=x;
		p=&(*p)->next;
	}
	*p=NULL;
	va_end(ap);
	return list;
}

List* listAppend(List* list,List* tail)
{
	List** p=&list;
	while(*p)//循环遍历
		p=&(*p)->next;
	*p=tail;
	return list;
}
List* listCopy(List* list)
{
	List* head;
	List** p=&head;
	for(;list;list=list->next)
	{
		p=malloc(sizeof(p));
		(*p)->data=list->data;
		p=&(*p)->next;
	}
	*p=NULL;
	return head;
}
//以上都是使用双重指针

List* listPop(List* list,void** x)
{
	if(list)
	{
		List* head=list->next;
		if(x)
			*x=list->data;
		free(list);
		list=NULL;
		return head;
	}
	else
		return list;
}
List* listReverse(List* list)
{
	List* head=NULL,*next;
	for(;list;list=next)
	{
		next=list->next;
		list->next=head;
		head=list;
	}
	return head;
}
int listLength(List* list)
{
	int n;
	for(n=0;list;list=list->next)
		++n;
	return n;
}
void listFree(List** list)
{
	List* next;
	assert(list);
	for(;*list;*list=next)
	{
		next=(*list)->next;
		free(*list);
	}
}
void listMap(List* list,void apply(void** x,void* cl),void* cl)
{
	assert(apply);
	for(;list;list=list->next)
		apply(&list->data,cl);
}
void** listToArray(List* list,void* end)
{
	int i,n=listLength(list);
	void** array=malloc((n+1)*sizeof(*array));
	for(i=0;i<n;++i)
	{
		array[i]=list->data;
		list=list->next;
	}
	array[i]=end;
	return array;
}
void listPrint(List* list)
{
	List* p=list;
	for(;p;p=p->next)
		printf("%s ",(char*)p->data);
	printf("\n");
}

int main()
{
	
	List* l=listPush(NULL,"This");
	l=listPush(l,"is");
	l=listPush(l,"a");
	l=listPush(l,"linked");
	l=listPush(l,"list");
	listPrint(l);
	int i=listLength(l);
	printf("%d\n",i);
	listFree(&l);

	List* d=listList("This","is","a","linked","list",NULL);
	listPrint(d);
	i=listLength(d);
	printf("%d\n",i);
	List* p=listReverse(d);
	listPrint(p);
	i=listLength(p);
	printf("%d\n",i);
	listFree(&d);
}
