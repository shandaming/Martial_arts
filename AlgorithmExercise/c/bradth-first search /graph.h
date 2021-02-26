#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>
#include "list.h"

#define INF -1
#define NIL 0
#define WHITE 1
#define GRAY 2
#define BLACK 3

#ifdef __cplusplus
extern C
{
#endif

//构造、析构
graphG* newGrpah(int n);
void freeGraph(graphG* pG);
//访问
int getOrder(graphG* G);
int getSize(graphG* G);
int getSource(graphG* G);
int getParent(graphG* G,int u);
int getDist(graphG* G,int u);
void getPath(List* L,graphG* G,int u);
//操作
void makeNull(graphG* G);
void addEdge(graphG* G,int u,int v);
void insertionSort(List* L,int u);
void addArc(graphG* G,int u,int v);
void BFS(graphG* G,int s);
//其他操作
void printGraph(FILE* out,graphG* G);

#ifndef __cplusplus
}
#endif

#endif
