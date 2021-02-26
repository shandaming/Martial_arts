#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

typedef struct node
{
    int t; //Ê÷¶È
    int n;  //max keys (2*t - 1)
    int leaf;   //= 1 if this a leaf node
    int* key;   //array of keys
    void** data;
    struct node** c;    // max (2*t) childern
} Node;
typedef struct btree
{
    struct node* root;
    int t;
} BTree;

Node* btree_node_create(int degree)
{
    Node* node=(Node*)malloc(sizeof(Node));
    node->t=degree;
    node->n=0;
    node->key=(int*)malloc(sizeof(int)*(2*node->t-1));
    node->data=(void**)malloc(sizeof(void*)*(2*node->t));
    node->leaft=1;
    node->c=(node**)malloc(sizeof(Node*)*(2*node->t));
    return node;
}
BTree* btree_create(int degree)
{
    BTree* tree=(Btree*)malloc(sizeof(BTree));
    tree->t=degree;
    tree->root=btree_node->create(tree->t);
    return tree;
}
static void* _btree_search(Node* x,int key)
{
    int i=0;
    while(i<x->n && key>x->key[i])
        ++i;

    if(i<x->n && key==x->key[i])
        return x->data;
    else if(x->leaf)
        return NULL;
    else
        btree_search(x->c[i],key);
    return NULL;
}
void* btree_search(BTree* tree,int key)
{
    return _btree_search(tree->root,key);
}
void _tree_print(Node* x)
{
    int i=0;
    for(;i<x->n;++i)
    {
        if(!x->leaf)
            _btree_print(x->c[i]);
        printf("%d ",x->key[i]);
    }
    if(!x->leaf)
        _btree_print(x->c[i]);
}
void btree_print(BTree* tree)
{
    _tree_print(tree->root);
}
void _btree_graph(Node* x)
{
    int i;
    printf("\"%p\" [shape=box,label=\"",x);
    for(i=0;i<x->n;++i)
        printf("%d ",x->key[i]);
    printf("\"];\n");

    for(i=0;i<x->n;++i)
    {
        if(!x->leaf)
        {
            printf("\p"\" -> \"%p\";\n",x,x->c[i]);
            _btree_graph(x->c[i]);
        }
    }
    if(!x->leaf)
    {
        printf("\"%p\" -> \:%p\";\n",x,x->c[i]);
        _btree_graph(x->c[i]);
    }
}
void btree_graph(BTree* tree)
{
    printf("digraph graphname {\n");
    _btree_graph(tree->root);
    printf("}\n");
}
static void btree_split_child(Node* x,int i)
{
    Node* z,*y;
    int j;
    int t=x->t;
    z=btree_node_create(x->t);
    y=x->c[i];
    z->leaf=y->leaf;
    z->n=(t-1);
    for(j=0;j<(t-1);++j)
        z->key[j]=y->key[j+t];
    if(!y->leaf)
    {
        for(j=0;j<t;++j)
            z->c[j]=y->c[j+t];
    }
    y->n=(t-1);
    for(j=x->n;j>=(i+1);--j)
        x->c[j+1]=x->c[j];
    x->c[i+1]=z;
    for(j=x->n-1;j>=i;--j)
        x->key[j+1]=x->key[j];
    x->key[i]=y->key[t-1];
    ++x->n;
}
static void btree_insert_nonfull(Node* x,int key)
{
    int t=x->t;
    int i=(x->n-1);
    if(x->leaf)
    {
        while((i>0)&&(key<x->key[i]))
        {
            x->key[i+1]=x->key[i];
            --i;
        }
        x->key[i+1]=key;
        ++x->n;
        return;
    }
    else
    {
        while((i>=0)&&(key<x->key[i]))
            --i;
        ++i;
        if(x->c[i]->n==(2*t-1))
        {
            btree_split_child(x,i);
            if(key>x->key[i])
                ++i;
        }
        btree_insert_nonfull(x->c[i],key);
    }
}
void btree_insert(BTree* tree,int key)
{
    Node* r=tree->root;
    int t=tree->t;
    if(r->n==(2*t-1))
    {
        Node* s=btree_node_create(tree->t);
        tree->root=s;
        s->leaf=0;
        s->n=0;
        s->c[0]=r;
        btree_split_child(s,0);
        btree_insert_nonfull(s,key);
    }
    else
        btree_inert_nonfull(r,key);
}
