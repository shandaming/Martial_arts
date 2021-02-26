#include <stdio.h>

#include "util.h"

//#define PARENT(i) (i / 2)
#define LEFT(i) (i * 2 + 1)
#define RIGHT(i) (LEFT(i) + 1)

void max_heapify(int* arr, int len, int i)
{
	int left = LEFT(i);
	int right = RIGHT(i);
	int largest = i;

	if(left < len && arr[i] < arr[left])
		largest = left;

	if(right < len && arr[largest] < arr[right])
		largest = right;

	if(i != largest)
	{
		swap(&arr[largest], &arr[i]);

		max_heapify(arr, len, largest);
	}
}
void build_max_heap(int* arr,int len)
{
	for(int i = (len / 2 - 1); i >= 0; --i)
	{
		max_heapify(arr, len, i);
	}
}

void heap_sort(int* arr,int len)
{
	build_max_heap(arr, len);

	for(int i = (len - 1); i >= 0; --i)
	{
		swap(&arr[0], &arr[i]);

		--len;

		max_heapify(arr, len, 0);
	}
}
/*
int heap_maximum(const int* arr) { return arr[0]; }

int heap_extract_max(int* arr, int len)
{
	if(len < 0)
		return -1;

	int max = heap_maximum(arr);
	arr[0] = arr[len - 1];
	max_heapify(arr, len - 1, 0);

	return max;
}
*/
int main()
{
	int arr[]={16,14,1,8,7,9,3,2,4,1,12};
	int len = ARRAY_LENGTH(arr);
	
	print(arr, len);
	build_max_heap(arr, len);
	print(arr, len);

	heap_sort(arr,len);
	print(arr, len);
}
