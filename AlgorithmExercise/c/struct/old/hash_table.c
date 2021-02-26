#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <stddef.h>

typedef struct hash_table
{
	int size;
	int (*cmp)(const void* x,const void* y);
	unsigned (*hash)(const void* key);
	int length;
	unsigned timestamp;

	struct binding
	{
		struct binding* link;
		const void* key;
		void* value;
	}** buckets;
} hash_table;

static inline int cmpatom(const void* left, const void* right)
{
	return left != right;
}
static inline unsigned hashatom(const void* key)
{
	return (unsigned long)key >> 2;
}

hash_table* table_new(int hint, int cmp(const void*, const void*), unsigned hash(const void* key))
{
	static int primes[] = 
	{
		509, 509, 1021, 2053, 4093,
		8191, 16381, 32771, 65521, INT_MAX
	};

	assert(hint >= 0);

	int i = 1;
	while(primes[i] < hint)
		++i;

	hash_table* table = malloc(sizeof(hash_table) + primes[i - 1] * sizeof(table->buckets[0]));
	if(!table)
		return NULL;

	table->size = primes[i - 1];
	table->cmp = cmp ? cmp : cmpatom;
	table->hash = hash ? hash : hashatom;
	table->buckets = (struct binding**)(table + 1);

	for(i = 0; i < table->size; ++i)
		table->buckets[i] = NULL;

	table->length = 0;
	table->timestamp = 0;

	return table;
}

void* table_get(hash_table* table, const void* key)
{
	assert(table && key);

	struct binding* p;
	int idx = (*table->hash)(key) % table->size;

	for(p = table->buckets[idx]; p; p = p->link)
	{
		if((*table->cmp)(key, p->key) == 0)
			break;
	}

	return p ? p->value : NULL;
}

//先查找，没有就插入，成功返回NULL;找到直接替换，返回被替换的值。
void* table_put(hash_table* table, const void* key, void* value)
{
	assert(table && key);

	int i = (*table->hash)(key) % table->size;
	struct binding* p;

	for(p = table->buckets[i]; p; p = p->link)
	{
		if((*table->cmp)(key, p->key) == 0)
			break;
	}

	void* prev;
	if(!p)
	{
		p = malloc(sizeof(struct binding));
		assert(p);

		p->key = key;
		p->link = table->buckets[i];
		table->buckets[i] = p;
		++table->length;
		prev = NULL;
	}
	else
		prev = p->value;

	p->value = value;
	++table->timestamp;

	return prev;
}

//返回实例的数目
int table_length(hash_table* table)
{
	assert(table);
	return table->length;
}

//有问题
void table_map(hash_table* table, void apply(const void*, void**, void*), void* cl)
{
	assert(table && apply);

	unsigned stamp = table->timestamp;

	for(int i = 0; i < table->size; ++i)
	{
		for(struct binding* p = table->buckets[i]; p; p = p->link)
		{
			apply(p->key, &p->value, cl);

			assert(table->timestamp == stamp);
		}
	}
}

void* table_remove(hash_table* table, const void* key)
{
	assert(table && key);

	++table->timestamp;
	int i = (*table->hash)(key) % table->size;

	for(struct binding** pp = &table->buckets[i]; *pp; pp = &(*pp)->link)
	{
		if((*table->cmp)(key, (*pp)->key) == 0)
		{
			struct binding* p = *pp;
			void* value = p->value;
			*pp = p->link;//????

			free(p);

			--table->length;
			return value;
		}
	}
	return NULL;
}

void** table_to_array(hash_table* table, void* end)
{
	assert(table);

	void** array = malloc((2 * table->length + 1) * sizeof(*array));
	int j = 0;

	for(int i = 0; i < table->size; ++i)
	{
		for(struct binding* beg = table->buckets[i]; beg; beg = beg->link)
		{
			array[j++] = (void*)beg->key;
			array[j++] = beg->value;
		}
	}

	array[j] = end;

	return array;
}

void table_free(hash_table** table)
{
	assert(table && *table);

	if((*table)->length > 0)
	{
		for(int i = 0; i < (*table)->size; ++i)
		{
			struct binding* iter;

			for(struct binding* beg = (*table)->buckets[i]; beg; beg = iter)
			{
				iter = beg->link;
				free(beg);
			}
		}
	}

	free(*table);
}

//----------------------------------------------------
void vfree(const void* key,void** count,void* cl)
{
	free(*count);
}
int main()
{
	hash_table* table=table_new(509,NULL,NULL);
	char* s=(char*)table_put(table,"我是","谁");
	char* t=(char*)table_put(table,"我是","你妈");
	char* c=(char*)table_put(table,"我是","456");
	char* d=(char*)table_put(table,"ID","号码");
	int i=table_length(table);
	printf("%s %s %s %s %d\n",s,t,c,d,i);
	char* c1=(char*)table_get(table,"我是");
	char* c2=(char*)table_get(table,"ID");
	printf("%s %s\n",c1,c2);
	char* r1=(char*)table_remove(table,"woshi");
	char* r2=(char*)table_remove(table,"我是");
	char* r3=(char*)table_remove(table,"ID");
	i=table_length(table);
	printf("%s %s %s %d\n",r1,r2,r3,i);
	//table_map(table,vfree,NULL);
	table_free(&table);
}
