#include "list.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
	char name[1024];

	struct list_head list;
} data;

int main(int argc, char* argv[])
{
	LIST_HEAD(head);

	for(int i = 0; i < 5; ++i)
	{
		data* d = malloc(sizeof(data));
		strcpy(d->name, "123321");

		list_add_tail(&(d->list), &head);
	}

	data* pos = NULL;
	list_for_each_entry(pos, &head, list)
	{
		printf("%s\n", pos->name);
	}
	return 0;
}
