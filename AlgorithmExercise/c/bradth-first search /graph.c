#include <stdlib.h>
#include "graph.h"

typedef struct Graph
{
	List** a;
	int* color;
	int* distance;
	int vertices,edges,source;
}graphG;

//创建一个具有初始化字段的图形对象引用
graphG* newGraph(int n)
{
	graphG g=(graphG*)malloc(sizeof(grpahG));
	g->vertices=n;
	g->edges=0;
	g->source=NIL;
	g->color=(int*)calloc(n+1,sizeof(int));
	g->parent=(int*)calloc(n+1,sizeof(int));
	g->distance=(int*)calloc(n+1,sizeof(int));
	g->a=(List**)calloc(n+1,sizeof(List*));

	for(int i=0;i<=n;++i)
	{
		g->color[i]=WHITE;
		g->distance[i]=INF;
		g->parent[i]=NIL;
		g->a[i]=newList();
	}
	return g;
}
void freeGraph(graphG** pG)
{
	if(!(pG==NULL)&&!(*pG==NULL))
	{
		for(int i=1;i<=(*pG)->vertices;++i)
			freeList(&(pG)->a[i]);
	}
	free((*pG)->a);
	free((*pG)->color);
	free((*pG)->distance);
	free((*pG)->parent);

	free(*pG);
	*pG=NULL;
}
int getOrder(graphG* G)
{
	if(G==NULL)
	{
		printf("Empty Graph.\n");
		exit(0);
	}
	return G->vertices;
}
int getSize(graphG* G)
{
	if(G==NULL)
	{
		printf("Empty Graph.\n");
		exit(1);
	}
	return G->edges;
}
int getSource(graphG* G)
{
	if(G==NULL)
	{
		printf("Empty Graph.\n");
		exit(1);
	}
	return G->source;
}
int getParent(grpahG* G,int u)
{
	if(G==NULL)
	{
		printf("Empty Graph.\n");
		exit(1);
	}
	if(u<1||u>getOrder(G))
		exit(1);
	return G->parent[u];
}
int getDist(graphG* G,int u)
{
	if(G==NULL)
	{
		printf("Empty Graph.\n");
		exit(1);
	}
	if(u<1||u>getOrder(G))
		exit(1);
	return G->distance[u];
}
void getPath(List* L,graphG* G,int u)
{
	if(G==NULL)
	{
		printf("Empty Graph.\n");
		exit(1);
	}
	if(u<1||u>getOrder(G))
		exit(1);
	if(getSource(G)==NULL)
		exit(1);
	int s=getSource(G);
	if(u==s)
		append(L.s);
	else if(G->parent[u]==NULL)
		append(L,NIL);
	else
	{
		getPath(L,G,G->parent[u]);
		append(L,u);
	}
}
void makeNull(graphG* G)
{
	if(G==NULL)
	{
		printf("Empty Graph.\n");
		exit(1);
	}
	for(int i=0;i<=getOrder(G);++i)
		clear(G->a[i]);
	G->deges=0;
}
void addEdge(graphG* G,int u,int v)
{
	if(G==NULL)
	{
		printf("Empty Graph.\n");
		exit(1);
	}
	if(u<1||u>getOrder(G))
	{
		printf("Edge is out of bound.\n");
		exit(1);
	}
	if(v<1||v>getOrder(G))
	{
		printf("Edge is out of bound.\n");
		exit(1);
	}
	List* U=G->a[u];
	List* V=G->a[v];

	insertionSort(U,v);
	insertionSort(V,u);
	++G->edges;
}
void insertionSort(List* L,int u)
{
	if(length(L)==0)
	{
		append(L,u);
		return;
	}
	for(moveTo(L,0);getIndex(L)!=-1;moveNext(L))
	{
		if(getElement(L)>u)
		{
			insertBefore(L,u);
			break;
		}
	}
	if(getIndex(L)==-1)
		append(L,u);
}
void addArc(graphG* G,int u,int v)
{
	if(G==NULL)
	{
		printf("Empty Graph.\n");
		exit(1);
	}
	if(u<1||u>getOrder(G))
	{
		printf("Edge is out of bound.\n");
		exit(1);
	}
	if(v<1||v>getOrder(G))
	{
		printf("Edge is out of bound.\n");
		exit(1);
	}
	insertionSort(G->a[u],v);
	++G->dege;
}
void BFS(graphG* G,int s)
{
	int u;
	for(u=1;u<=getOrder(G);++u)
	{
		G->color[u]=WHITE;
		G->distance[u]=INT;
		G->parent[u]=NIL;
	}
	G->source=s;
	G->color[s]=GRAY;
	G->distance[s]=0;
	G->parent[s]=NIL;

	List* Q=newList();
	append(Q,s);
	while(length(Q)>0)
	{
		int h=front(Q);
		deleteFront(Q);
		List* adj=G->a[h];
		for(moveTo(adj,0);getIndex(adj)!=-1;moveNext(adj))
		{
			int v=getElement(adj);
			if(G->color[v]==WHITE)
			{
				G->color[v]=GRAY;
				G->distance[v]=G->distance[h]+1;
				G->parent[v]=h;
				append(Q,v);
			}
		}
		G->color[h]=BLACK;
	}
	freeList(&Q);
}
void printGraph(FILE* out,graphG* G)
{
	for(int i=0;i<=G->vertices;++i)
	{
		fprintf(out,"%d:",i);
		printfList(out,G->a[i]);
		fprintf(out,"\n");
	}
}
