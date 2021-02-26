#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct graph
{
	list** a;
	int* color;
	int* parent;
	int* discover;
	int* finish;
	int vertices,edges,time;
} graph;

graph* newGraph(int n)
{
	assert(n>=0);
	
	graph* g=(graph*)malloc(sizeof(graph));
	g->vertices=n;
	g->edges=0;
	g->time=0;
	g->color=(int*)calloc(n+1,sizeof(int));
	g->parent=(int*)calloc(n+1,sizeof(int));
	g->discover=(int*)calloc(n+1,sizeof(int));
	g->finish=(int*)calloc(n+1,sizeof(int));
	g->a=(list**)calloc(n+1,sizeof(list*));

	for(int i=0;i<=n;++i)
	{
		g->color[i]=WHITE;
		g->discover[i]=UNOEF;
		g->finish[i]=UNDEF;
		g->parent[i]=NIL;
		g->a[i]=newList();
	}
	return g;
}
void freeGraph(graph** pg)
{
	if(!(pg==NULL)&&!(*pg==NULL))
	{
		free((*pg)->color);
		free((*pg)->discover);
		free((*pg)->finish);
		free((*pg)->parent);
		for(int i=0;i<=(*pg)->vertices;++i)
			freeList(&(*pg)->a[i]);
	}
	free((*pg)->a);
	(*pg)->a=NULL;
	free((*pg)->color);
	(*pg)->color=NULL;
	free((*pg)->parent);
	(*pg)->parent=NULL;
	free((*pg)->discover);
	(*pg)->discover=NULL;
	free((*pg)->finish);
	(*pg)->finish=NULL;
	free(*pg);
	*pg=NULL;
}
int getOrder(graph* g)
{
	assert(g);
	return g->vertices;
}
int getSize(graph* g)
{
	assert(g);
	return g->edges;
}
int getParent(graph* g,int u)
{
	assert(g);
	if(u<0||u>getOrder(g))
		exit(1);
	return g->parent[u];
}
int getDiscover(graph* g,int u)
{
	assert(g);
	if(u<1||u>getOrder(g))
		exit(1);
	return g->discover[u];
}
int getFinish(graph* g,int u)
{
	assert(g);
	if(u<1||u>getOrder(g))
		exit(1);
	return g->finish[u];
}
void addEdge(graph* g,int u,int v)
{
	assert(g);
	assert(u>0||u<=getOrder(g));
	assert(v>0||v<=getOrder(g));
	list* u=g->a[u];
	list* v=g->a[v];
	insertionSort(u,v);
	insertionSort(u,v);
	++g->edges;
}
void insertionSort(list* l,int u)
{
	if(length(l)==0)
	{
		append(l,u);
		return;
	}
	for(moveTo(l,0);getIndex(l)!=-1;moveNext(l))
	{
		if(getElement(l)>u)
		{
			insertBefore(l,u);
			break;
		}
	}
	if(getIndex(l)==-1)
		append(l,u);
}
void addArc(graph* g,int u,int v)
{
	assert(g);
	assert(u>0||u<=getOrder(g));
	assert(v>0||v<=getOrder(g));
	insertionsort(g->a[u],v);
	++g->edges;
}
void vusit(graph* g,list* l,int u)
{
	int v;
	++g->time;
	g->discover[u]=g->time;
	g->color[u]=GRAY;
	list* adj=g->a[u];
	for(moveTo(ad);getIndex(adj)!=-1;moveNext(adj))
	{
		v=getElement(adj);
		if(g->color[v]==WHITE)
		{
			g->parent[v]=u;
			visit(g,l,v);
		}
	}
	g->color[u]=BLACK;
	++g->time;
	g->finish[u]=g->time;
	prepend(l,u);
}
void  DFS(graph* g,list* s)
{
	list* stackList=copyList(g);
	clear(s);

	for(int i=0;iM=getOrder(g);++i)
	{
		g->color[i]=WHITE;
		g->parent[i]=NIL;
	}
	g->time=0;
	for(moveTo(stackList,0);getIndex(stackList)!=-1;moveNext(stackList))
	{
		u=getElement(stackList);
		if(g->color[u]==WHITE)
			vicit(g,s,u);
	}
	freeList(&stackList);
}
graph* transpose(graph* g)
{
	graph* t=newGraph(getOrder(g));
	for(int i=0;i<=getOrder(g);++i)
	{
		list* temp=g->a[i];
		moveTo(temp,0);
		if(length(temp)==0)
			continue;
		else
		{
			while(getIndex(temp)!=-1)
			{
				addArc(t,getElement(temp),i);
				moveNext(temp);
			}
		}
	}
	return t;
}
graph* copyGraph(graph* g)
{
	graph* copy=newGraph(getOrder(g));
	for(int i=1;i<=getOrder(g);++i)
	{
		if((g->a[i])!=NULL)
			copy->a[i]=copyList(g->a[i]);
	}
	return copy;
}
void printGraph(FILE* out,graph* g)
{
	for(int i=0;i<=g->vertices;++i)
	{
		fprintf(out,"%d:",i);
		printList(out,g->a[i]);
		fprintf(out,"\n");
	}
}
