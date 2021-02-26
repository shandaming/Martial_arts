/*
 * Copyright (C) 2020
 */

#include <stdio.h>

#include "util.h"

void print(const int* array, int len)
{
	for(int i = 0; i != len; ++i)
	{
		printf("%d ", array[i]);
	}
	puts("\n");
}

void swap(int* left, int* right)
{
	int tmp = *left;
	*left = *right;
	*right = tmp;
}

int get_array_max_value(int arr[], int len)
{
	int max = arr[0];

	for(int i = 1; i != len; ++i)
		if(arr[i] > max)
			max = arr[i];

	return max;
}
