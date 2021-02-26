/*
 * Copyright (C) 2020
 */

#include "util.h"

void bubble_sort(int* arr, int len)
{
	for(int i = 0; i != len; ++i)
	{
		for(int j = len; j > i; --j)
		{
			if(arr[j] < arr[j - 1])
				swap(&arr[j], &arr[j - 1]);
		}
	}
}

int main(int argc, char* argv[])
{
	int array[] = {2, 4, 5, 7, 1, 2, 3, 6, 12, 9, 8};
	int len = ARRAY_LENGTH(array);

	print(array, len);
	bubble_sort(array, len);
	print(array, len);
}
