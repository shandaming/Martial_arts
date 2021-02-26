/*
 * Copyright (C) 2020
 */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "util.h"

int partition(int* arr,int beg, int end)
{
	int pivot_element = arr[end];
	int i = beg - 1;

	for(int j = beg; j != end; ++j)
	{
		if(arr[j] <= pivot_element)
			swap(&arr[++i], &arr[j]);
	}

	swap(&arr[++i], &arr[end]);

	return i;
}
void quick_sort(int* arr, int beg, int end)
{
	if(beg < end)
	{
		int index = partition(arr, beg, end);
		quick_sort(arr, beg, index - 1);
		quick_sort(arr, index + 1, end);
	}
}
void tail_recursive_quick_sort(int arr[], int beg, int end)
{
	while(beg < end)
	{
		int index = partition(arr, beg, end);
		tail_recursive_quick_sort(arr, beg, index - 1);
		beg = index + 1;
	}
}


//////----------------------------
int get_random(int beg, int end)
{
	srand((unsigned)time(NULL));
	return beg + (rand() % (end - beg + 1));
}
int random_partition(int arr[], int beg, int end)
{
	int random_index = get_random(beg, end);
	swap(&arr[end], &arr[random_index]);
	return partition(arr, beg, end);
}
void random_quick_sort(int arr[], int beg, int end)
{
	if(beg < end)
	{
		int index = random_partition(arr, beg, end);
		random_quick_sort(arr, beg, index - 1);
		random_quick_sort(arr, index + 1, end);
	}
}

int main(int argc, char* argv[])
{
	int array[] = {13, 19, 9, 5, 12, 8, 7, 4, 21, 2, 6, 11};
	int len = ARRAY_LENGTH(array);

	print(array, len);
	quick_sort(array, 0, len - 1);
	print(array, len);

	int array1[] = {13, 19, 9, 5, 12, 8, 7, 4, 21, 2, 6, 11};
	print(array1, len);
	tail_recursive_quick_sort(array1, 0, len -1);
	print(array1, len);

	int array2[] = {13, 19, 9, 5, 12, 8, 7, 4, 21, 2, 6, 11};
	print(array2, len);
	random_quick_sort(array2, 0, len - 1);
	print(array2, len);
}
