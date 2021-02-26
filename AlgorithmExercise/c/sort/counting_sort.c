/*
 * Copyright (C) 2020
 */

#include <assert.h>
#include <stdlib.h>

#include "util.h"

void counting_sort(int arr[], int len, int max_value)
{
	int count[max_value + 1];

	for(int i = 0; i != max_value + 1; ++i)
		count[i] = 0;

	// 开始计数
	for(int i = 0; i != len; ++i)
		++count[arr[i]];

	// 相邻2数相加
	for(int i = 1; i != max_value + 1; ++i)
		count[i] += count[i - 1];

	int result[len];

	for(int i = len - 1; i >= 0; --i)
	{
		result[count[arr[i]] - 1] = arr[i];
		--count[arr[i]]; // 递减计数
	}

	for(int i = 0; i != len; ++i)
		arr[i] = result[i];
}

int main(int argc, char* argv[])
{
	int array[] = {2, 5, 3, 0, 2, 3, 0, 3, 4, 7, 3, 6, 9, 0, 4, 8};
	int len = ARRAY_LENGTH(array);

	print(array, len);
	counting_sort(array, len, get_array_max_value(array, len));
	print(array, len);
}
