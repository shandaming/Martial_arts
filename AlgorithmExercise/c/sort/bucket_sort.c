/*
 * Copyright (C) 2020
 */

#include "util.h"
#include "vector.h"
#include <stdio.h>

void insertion_sort(float* arr,int len)
{
	if(!arr)
		return;

	for(int i = 1; i != len; ++i)
	{
		float key = arr[i];
		int j = i - 1;

		while(j > -1 && arr[j] > key)
		{
			arr[j + 1] = arr[j];
			--j;
		}
		arr[j + 1] = key;
	}
}

void bucket_sort(float arr[], int len)
{
	float* bucket[len];

	for(int i = 0; i != len; ++i)
		bucket[i] = NULL;

	for(int i = 0; i != len; ++i)
	{
		int index = len * arr[i];
		vector_push_back(bucket[index], arr[i]);
	}

	for(int i = 0; i != len; ++i)
		insertion_sort(vector_begin(bucket[i]), get_vector_size(bucket[i]));

	for(int i = 0, index = 0; i != len; ++i)
	{
		float* list = bucket[i];
		while(list && !vector_is_empty(list))
		{
			arr[index++] = *(vector_begin(list));
			vector_erase(list, 0);
		}
	}
}

int main(int argc, char* argv[])
{
	float array[] = {0.897, 0.565, 0.656, 0.1234, 0.665, 0.3434};
	int len = ARRAY_LENGTH(array);

	for(int i = 0; i != len; ++i)
		printf("%f ", array[i]);
	printf("\n");

	bucket_sort(array, len);
	
	for(int i = 0; i != len; ++i)
		printf("%f ", array[i]);
	printf("\n");
}
