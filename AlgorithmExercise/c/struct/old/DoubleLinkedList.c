#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
//c++里编译通过
typedef struct node
{
    const void* data;
    struct node* prev;
    struct node* next;
} Node;
typedef struct list
{
    struct node* head;
    struct node* tail;
} List;

List* listInit(List* l)
{
    l->head=l->tail=0;
}
void pushBack(List* l,const void* data)
{
    Node* node=(Node*)malloc(sizeof(Node));
    assert(node);
    node->data=data;
    if(l->tail)
    {
        l->tail->next=node;
        node->prev=l->tail;
        node->next=NULL;
        l->tail=node;//node变成tail
    }
    else
    {
        //头尾都是node
        l->head=node;
        l->tail=node;
        node->prev=NULL;
        node->next=NULL;
    }
}
void pushFront(List* l,const void* data)
{
    Node* node=(Node*)malloc(sizeof(Node));
    assert(node);
    node->data=data;
    if(l->head)
    {
        l->head->prev=node;
        node->next=l->head;
        node->prev=NULL;
        l->head=node;
    }
    else
    {
        l->head=node;
        l->tail=node;
        node->prev=NULL;
        node->next=NULL;
    }
}
//在index后插入数据
void listInsert(List* l,int index,const void* data)
{
    Node* nodeTemp=l->head;
    for(int i=0;i!=index;++i)//根据所有来移动nodeTemp
        nodeTemp=nodeTemp->next;

    Node* node=l->head;
    if(l==NULL)
        pushBack(l,data);
    else
    {
        while(node!=NULL)
        {
            if(node==nodeTemp)
            {
                Node* newNode=(Node*)malloc(sizeof(Node));
                assert(newNode);
                newNode->data=data;
                newNode->prev=node;
                if(node->next)
                {
                    newNode->next=node->next;
                    node->next->prev=newNode;
                }
                else
                    newNode->next=NULL;
                node->next=newNode;
                if(nodeTemp==l->tail)
                    l->tail=newNode;
                break;
            }
            node=node->next;
        }
    }
}
int listSize(List* l)
{
    int n=0;
    Node* node=l->head;
    while(node!=NULL)
    {
        ++n;
        node=node->next;
    }
    return n;
}
void listDelete(List* l,int index/*Node* node*/)
{
    Node* node=l->head;
    for(int i=0;i!=index;++i)
        node=node->next;

    Node* prev=node->prev;
    Node* next=node->next;
    if(prev)
    {
        if(next)
        {
            prev->next=next;
            next->prev=prev;
        }
        else
        {
            prev->next=0;
            l->tail=prev;
        }
    }
    else
    {
        if(next)
        {
            next->prev=0;
            l->head=next;
        }
        else
        {
            l->head=0;
            l->tail=0;
        }
    }
    free(node);
}
void listRemoveHead(List* l)
{
    assert(l);
    assert(l->head);
    Node* node=l->head;
    Node* next=node->next;
    if(next)
    {
        next->prev=NULL;
        l->head=next;
    }
    else
    {
        l->head=NULL;
        l->tail=NULL;
    }
    free(node);
}
void listRemoveTail(List* l)
{
    assert(l);
    assert(l->tail);
    Node* node=l->tail;
    Node* prev=node->prev;
    if(prev)
    {
        prev->next=NULL;
        l->tail=prev;
    }
    else
    {
        l->tail=NULL;
        l->head=NULL;
    }
    free(node);
}
Node* listSearch(List* l,const void* target,int (*cmp)(const void* a,const void* b))
{
    Node* node=l->head;
    while(node!=NULL)
    {
        if(cmp(node->data,target)==0)
            return node;
        node=node->next;
    }
    return NULL;
}
//链表遍历:linked_list_traverse
void listTraverse(List* l,void* data,void (*callback)(void*,void*))
{
    for(Node* node=l->head;node;node=node->next)
        callback(data,(void*)node->data);
}
//链接列表遍历:linked list traverse in reverse
void listReverse(List* l,void* data,void (*callback)(void*,void*))
{
    for(Node* node=l->tail;node;node=node->prev)
        callback(data,(void*)node->data);
}
//linked list traverse delete链表遍历删除
void linked_list_traverse_delete(List* l,int (*callback)(void*))
{
    for(Node* node=l->head,*next;node;node=next)
    {
        next=node->next;
        if(callback((void*)node->data))
        {
            for(int i=0;i!=listSize(l);++i)
                listDelete(l,i);
        }
    }
}
void listFree(List* l)
{
    /*
    Node* node=l->head;//创建node节点指向头节点
    Node* next=NULL;
    //当node指向的内容不为空就循环删除节点
    while(node!=NULL)
    {
        next=node->next;
        free(node);
        node=next;
    }
    free(l);
    */

    for(Node* node=l->head,*next;node;node=next)
    {
        next=node->next;
        free(node);
    }
}
void printList(List* l)
{
    Node* node=l->head;
    while(node!=NULL)
    {
        printf("%s ",node->data);
        node=node->next;
    }
    printf("\n");
}
int main()
{
    List l;
    listInit(&l);
    pushFront(&l,"asd");
    pushFront(&l,"12fff3");
    pushFront(&l,"121233");
    pushFront(&l,"234");
    pushFront(&l,"3");
    pushFront(&l,"1gfdash");
    pushFront(&l,"sdfsdfds");
    pushFront(&l,"ffdddd");
    printList(&l);
    int i=listSize(&l);
    printf("%d\n",i);
    //listInsert(&l,1,"我们");
    //listDelete(&l,5);
    //listRemoveTail(NULL);
    Node* n=listSearch(&l,"ffdddd",(int (*)(const void*,const void*))strcmp);
    printf("%s\n",n->data);
    printList(&l);
    i=listSize(&l);
    printf("%d\n",i);
    listFree(&l);
}

/*参考资料1
 *list.h
 */
#include <stdlib.h>

typedef struct listNode {
    void *item;
    struct listNode *prev;
    struct listNode *next;
} listNode;

typedef struct list {
    struct listNode *head;
    struct listNode *tail;
} list;

list *list_init();
void list_free(list *l,void (*freeItem)(void *d));
void list_append(list *l, void *i);
void list_prepend(list *l, void *i);
int list_count(list *l);
listNode *list_nth(list *l, int n);
void list_insert(list *l, listNode *after, void *item);
void list_remove(list *l, listNode *remove, void (*freeItem)(void *d));
listNode *list_find(list *l, void *target, int (*cmp)(void *a,void *b));
void list_apply(list *l, void (*f)(void *d));
list *list_filter(list *l, int (*f)(void *d));
list *list_map(list *l, void *(*f)(void *d));
list *list_slice(list *l, int start, int end, void *(*dup) (void *i));
list *list_concat(list *l1, list *l2, void *(*dup) (void *i));
void list_debug(list *l, void(*pf) (void *i));
void list_sort(list *l, int(*cmp) (const void *i, const void *j));
//list.c
#define _GNU_SOURCE

#include <assert.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>

#include "list.h"

list *list_init() {
    list *l = malloc(sizeof(list));
    assert(l);
    l->head = NULL;
    l->tail = NULL;
    return l;
}

void list_free(list *l,void (*freeItem)(void *d)) {
    listNode *i = l->head;
    listNode *n;
    while (i != NULL) {
        if (freeItem != NULL) {
            freeItem(i->item);
        }
        n = i->next;
        free(i);
        i = n;
    }
    free(l);
}

void list_append(list *l, void *i) {
    listNode *n = (listNode *) malloc(sizeof(listNode));
    assert(n);
    listNode *t = l->tail;
    n->item = i;
    if (t == NULL) {
        n->prev = NULL;
        n->next = NULL;
        l->head = n;
        l->tail = n;
    } else {
        n->prev = t;
        n->next = NULL;
        t->next = n;
        l->tail = n;
    }
}

void list_prepend(list *l, void *i) {
    listNode *n = (listNode *) malloc(sizeof(listNode));
    assert(n);
    listNode *h = l->head;
    n->item = i;
    if (h == NULL) {
        n->prev = NULL;
        n->next = NULL;
        l->head = n;
        l->tail = n;
    } else {
        n->prev = NULL;
        n->next = h;
        h->prev = n;
        l->head = n;
    }
}

int list_count(list *l) {
    int n = 0;
    listNode *i = l->head;
    while (i != NULL) {
        ++n;
        i = i->next;
    }
    return n;
}

listNode *list_nth(list *l, int n) {
    listNode *i = l->head;
    int c = 0;
    if (n < 0) {
        n = list_count(l) + n;
    }
    while (i != NULL) {
        if (c == n) {
            return i;
        }
        ++c;
        i = i->next;
    }
    return NULL;
}

void list_insert(list *l, listNode *after, void *item) {
    listNode *n = l->head;
    while (n != NULL) {
        if (n == after) {
            listNode *new = (listNode *) malloc(sizeof(listNode));
            assert(new);
            new->item = item;
            new->prev = n;
            if (n->next) {
                new->next = n->next;
                n->next->prev = new;
            } else {
                new->next = NULL;
            }
            n->next = new;
            if (after == l->tail) {
                l->tail = new;
            }
            break;
        }
        n = n->next;
    }
}

void list_remove(list *l, listNode *remove, void (*freeItem)(void *d)) {
    listNode *n = l->head;
    while (n != NULL) {
        if (n == remove) {
            if (n->prev) {
                n->prev->next = n->next;
            }
            if (n->next) {
                n->next->prev = n->prev;
            }
            if (n == l->head) {
                l->head = n->next;
            }
            if (n == l->tail) {
                l->tail = n->prev;
            }
            if (freeItem != NULL) {
                freeItem(n->item);
            }
            free(n);
            break;
        }
        n = n->next;
    }
}

listNode *list_find(list *l, void *target, int (*cmp)(void *a,void *b)) {
    listNode *n = l->head;
    while (n != NULL) {
        if (cmp(n->item,target) == 0) {
            return n;
        }
        n = n->next;
    }
    return NULL;
}

void list_apply(list *l, void (*f)(void *d)) {
    listNode *n = l->head;
    while (n != NULL) {
        f(n->item);
        n = n->next;
    }
}

list *list_filter(list *l, int (*f)(void *d)) {
    list *r = list_init();
    listNode *n = l->head;
    while (n != NULL) {
        if (f(n->item)) {
            list_append(r,n->item);
        }
        n = n->next;
    }
    return r;
}

list *list_map(list *l, void *(*f)(void *d)) {
    list *r = list_init();
    listNode *n = l->head;
    while (n != NULL) {
        list_append(r,f(n->item));
        n = n->next;
    }
    return r;
}

list *list_slice(list *l, int start, int end, void *(*dup) (void *i)) {
    list *r = list_init();
    listNode *i = l->head;
    int c = 0;
    if (start < 0) {
        start = list_count(l) + start;
    }
    if (end < 0) {
        end = list_count(l) + end;
    }
    while (i != NULL) {
        if (c >= start && c <= end) {
            if (dup) {
                list_append(r,dup(i->item));
            } else {
                list_append(r,i->item);
            }
        }
        ++c;
        i = i->next;
    }
    return r;
}


list *list_concat(list *l1, list *l2, void *(*dup) (void *i)) {
    list *r = list_init();
    listNode *i = l1->head;
    while (i != NULL) {
        if (dup) {
            list_append(r,dup(i->item));
        } else {
            list_append(r,i->item);
        }
        i = i->next;
    }
    i = l2->head;
    while (i != NULL) {
        if (dup) {
            list_append(r,dup(i->item));
        } else {
            list_append(r,i->item);
        }
        i = i->next;
    }
    return r;
}

void list_debug(list *l, void(*pf) (void *i)) {
    printf("--- List [%p]: head -> %p / tail -> %p\n",l,l->head,l->tail);
    listNode *n = l->head;
    while (n != NULL) {
        printf("  + Item [%p]: data -> %p / prev -> %p / next -> %p\n",
                n,n->item,n->prev,n->next);
        if (pf != NULL) {
            pf(n->item);
        }
        n = n->next;
    }
}

void list_sort(list *l, int(*cmp) (const void *i, const void *j)) {
    /* Copy list into array */
    int i = 0;
    int nmemb = list_count(l);
    void **base = malloc(nmemb * sizeof(void *));
    bzero(base,nmemb * sizeof(void *));
    void **p = base;
    assert(base);
    listNode *n = l->head;
    while (n != NULL) {
        *p = n->item;
        ++p;
        n = n->next;
    }
    /* qsort array */
    qsort(base,nmemb,sizeof(void*),cmp);
    i = 0;
    n = l->head;
    p = base;
    while (n != NULL) {
        n->item = *p;
        ++p;
        n = n->next;
    }
    free(base);
}

/*参考资料2
 *
 */
#include <stdlib.h>
#include <stdio.h>
#ifdef DLLTEST
/* strchr, declared in string.h, is used in the examples. */
#include <string.h>
#define HEADER
#else
#include "dll.h"
#endif
#ifdef HEADER

/* The type link_t needs to be forward-declared in order that a
   self-reference can be made in "struct link" below. */

typedef struct link link_t;

/*  _ _       _
   | (_)_ __ | | __
   | | | '_ \| |/ /
   | | | | | |   <
   |_|_|_| |_|_|\_\ */


/* A link_t contains one of the links of the linked list. */

struct link {
    const void * data;
    link_t * prev;
    link_t * next;
};

/*  _ _       _            _     _ _     _
   | (_)_ __ | | _____  __| |   | (_)___| |_
   | | | '_ \| |/ / _ \/ _` |   | | / __| __|
   | | | | | |   <  __/ (_| |   | | \__ \ |_
   |_|_|_| |_|_|\_\___|\__,_|___|_|_|___/\__|
                           |_____|            */


/* linked_list_t contains a linked list. */

typedef struct linked_list
{
    int count;
    link_t * first;
    link_t * last;
}
linked_list_t;

#endif /* def HEADER */

/* The following function initializes the linked list by putting zeros
   into the pointers containing the first and last links of the linked
   list. */

void
linked_list_init (linked_list_t * list)
{
    list->first = list->last = 0;
    list->count = 0;
}

/*            _     _
     __ _  __| | __| |
    / _` |/ _` |/ _` |
   | (_| | (_| | (_| |
    \__,_|\__,_|\__,_| */


/* The following function adds a new link to the end of the linked
   list. It allocates memory for it. The contents of the link are
   copied from "data". */

void
linked_list_add (linked_list_t * list, const void * data)
{
    link_t * link;

    /* calloc sets the "next" field to zero. */
    link = calloc (1, sizeof (link_t));
    if (! link) {
        fprintf (stderr, "calloc failed.\n");
        exit (EXIT_FAILURE);
    }
    link->data = data;
    if (list->last) {
        /* Join the two final links together. */
        list->last->next = link;
        link->prev = list->last;
        list->last = link;
    }
    else {
        list->first = link;
        list->last = link;
    }
    list->count++;
}

/*      _      _      _
     __| | ___| | ___| |_ ___
    / _` |/ _ \ |/ _ \ __/ _ \
   | (_| |  __/ |  __/ ||  __/
    \__,_|\___|_|\___|\__\___| */



void
linked_list_delete (linked_list_t * list, link_t * link)
{
    link_t * prev;
    link_t * next;

    prev = link->prev;
    next = link->next;
    if (prev) {
        if (next) {
            /* Both the previous and next links are valid, so just
               bypass "link" without altering "list" at all. */
            prev->next = next;
            next->prev = prev;
        }
        else {
            /* Only the previous link is valid, so "prev" is now the
               last link in "list". */
            prev->next = 0;
            list->last = prev;
        }
    }
    else {
        if (next) {
            /* Only the next link is valid, not the previous one, so
               "next" is now the first link in "list". */
            next->prev = 0;
            list->first = next;
        }
        else {
            /* Neither previous nor next links are valid, so the list
               is now empty. */
            list->first = 0;
            list->last = 0;
        }
    }
    free (link);
    list->count--;
}

/*  _
   | |_ _ __ __ ___   _____ _ __ ___  ___
   | __| '__/ _` \ \ / / _ \ '__/ __|/ _ \
   | |_| | | (_| |\ V /  __/ |  \__ \  __/
    \__|_|  \__,_| \_/ \___|_|  |___/\___| */



void
linked_list_traverse (linked_list_t * list, void * data,
                      void (*callback) (void *, void *))
{
    link_t * link;

    for (link = list->first; link; link = link->next) {
        callback (data, (void *) link->data);
    }
}


/*  _ __ _____   _____ _ __ ___  ___
   | '__/ _ \ \ / / _ \ '__/ __|/ _ \
   | | |  __/\ V /  __/ |  \__ \  __/
   |_|  \___| \_/ \___|_|  |___/\___| */



void
linked_list_traverse_in_reverse (linked_list_t * list, void * data,
                                 void (*callback) (void *, void *))
{
    link_t * link;

    for (link = list->last; link; link = link->prev) {
        callback (data, (void *) link->data);
    }
}

/*  _
   | |_ _ __ __ ___   _____ _ __ ___  ___
   | __| '__/ _` \ \ / / _ \ '__/ __|/ _ \
   | |_| | | (_| |\ V /  __/ |  \__ \  __/
    \__|_|  \__,_| \_/ \___|_|  |___/\___|

     ___         _      _      _
    ( _ )     __| | ___| | ___| |_ ___
    / _ \/\  / _` |/ _ \ |/ _ \ __/ _ \
   | (_>  < | (_| |  __/ |  __/ ||  __/
    \___/\/  \__,_|\___|_|\___|\__\___| */



void
linked_list_traverse_delete (linked_list_t * list, int (*callback) (void *))
{
    link_t * link;
    link_t * next;

    for (link = list->first; link; link = next) {
	next = link->next;
        if (callback ((void *) link->data)) {
            linked_list_delete (list, link);
        }
    }
}

/*   __
    / _|_ __ ___  ___
   | |_| '__/ _ \/ _ \
   |  _| | |  __/  __/
   |_| |_|  \___|\___| */



/* Free the list's memory. */

void
linked_list_free (linked_list_t * list)
{
    link_t * link;
    link_t * next;
    for (link = list->first; link; link = next) {
        /* Store the next value so that we don't access freed
           memory. */
        next = link->next;
        free (link);
    }
}

#ifdef DLLTEST

/* Example traverse delete function - delete everything. */

static int
delete_all (void * data)
{
    return 1;
}

/* Example traverse delete function - delete things with an "a" in
   them. */

static int
delete_a (void * data)
{
    return (int) strchr ((char *) data, 'a');
}

/* Example callback function. */

static void
print_list (void * user_data, void * data)
{
    printf ("\t%s\n", (char *) data);
}

/* Make a list of words and then print them out. */

int main ()
{
    linked_list_t list;

    linked_list_init (& list);
    linked_list_add (& list, "dingo");
    linked_list_add (& list, "kangaroo");
    linked_list_add (& list, "koala");
    linked_list_add (& list, "kookaburra");
    linked_list_add (& list, "tasmanian devil");
    linked_list_add (& list, "wallabee");
    linked_list_add (& list, "wallaroo");
    linked_list_add (& list, "wombat");
    printf ("Australian animals, in reverse alphabetical order:\n");
    linked_list_traverse_in_reverse (& list, 0, print_list);
    printf ("\n");
    linked_list_traverse_delete (& list, delete_a);
    printf ("These Australian animals don't have an 'a' in their names:\n");
    linked_list_traverse (& list, 0, print_list);
    printf ("\n");
    linked_list_traverse_delete (& list, delete_all);
    printf ("All the Australian animals are now extinct:\n");
    linked_list_traverse (& list, 0, print_list);
    printf ("\n");
    linked_list_free (& list);

    return 0;
}

#endif /* def DLLTEST */

