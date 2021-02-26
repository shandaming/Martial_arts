/*
 * Copyright (C) 2020
 */

#include <stdlib.h>
#include <assert.h>

#include "util.h"

void merge(int* arr,int beg,int mid,int end)
{
	int left_length = mid - beg + 1;
	int right_length = end - mid;

	int* left_arr = malloc(sizeof(int) * left_length);
	int* right_arr = malloc(sizeof(int) * right_length);
	assert(left_arr && right_arr);

	for(int i = 0; i != left_length; ++i)
		left_arr[i] = arr[beg + i];

	for(int j = 0; j != right_length; ++j)
		right_arr[j] = arr[mid + 1 + j];

	int i = 0, j = 0, k = beg;
	while(i != left_length && j != right_length)
	{
		if(left_arr[i] <= right_arr[j])
			arr[k++]=left_arr[i++];
		else
			arr[k++]=right_arr[j++];
	}

	for(; i != left_length; ++k, ++i)
		arr[k] = left_arr[i];

	for(; j != right_length; ++k, ++j)
		arr[k] = right_arr[j];

	free(left_arr);
	free(right_arr);
}
void merge_sort(int* arr, int beg, int end)
{
	if(beg < end)
	{
		int mid = (beg + end) / 2;
		merge_sort(arr, beg, mid);
		merge_sort(arr, mid + 1, end);
		merge(arr, beg, mid, end);
	}
}

int main(int argc, char* argv[])
{
	int array[] = {2, 4, 5, 7, 1, 2, 3, 6, 12, 9, 8};
	int len = ARRAY_LENGTH(array);

	print(array, len);
	merge_sort(array, 0, len);
	print(array, len);
}
