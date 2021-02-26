#include <stdio.h>

#include "util.h"

void insertion_sort(int* arr,int len)
{
	for(int i = 1; i != len; ++i)
	{
		int key = arr[i];
		int j = i - 1;

		while(j > -1 && arr[j] > key)
		{
			arr[j + 1] = arr[j];
			--j;
		}
		arr[j + 1] = key;
	}
}

int main(int argc, char* argv[])
{
	int array[] = {5, 2, 4, 3, 6, 1};
	int len = ARRAY_LENGTH(array);

	print(array, len);
	insertion_sort(array, len);
	print(array, len);
}
