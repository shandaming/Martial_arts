/*
 * Copyright (C) 2020
 */

#include "util.h"

void count_sort(int arr[], int len, int exp)
{
	int count[10] = {0};

	// 开始计数
	for(int i = 0; i != len; ++i)
		++count[(arr[i] / exp) % 10];

	// 相邻2数相加
	for(int i = 1; i != 10; ++i)
		count[i] += count[i - 1];

	int result[len];

	for(int i = len - 1; i >= 0; --i)
	{
		result[count[(arr[i] / exp) % 10] - 1] = arr[i];
		--count[(arr[i] / exp) % 10]; // 递减计数
	}

	for(int i = 0; i != len; ++i)
		arr[i] = result[i];
}

void radix_sort(int arr[], int len)
{
	int max_value = get_array_max_value(arr, len);

	for(int exp = 1; max_value / exp > 0; exp *= 10)
		count_sort(arr, len, exp);
}

int main(int argc, char* argv[])
{
	int array[] = {22, 51, 3, 300, 22, 43, 50, 73, 84, 74, 33, 66, 90, 10, 14, 83};
	int len = ARRAY_LENGTH(array);

	print(array, len);
	radix_sort(array, len);
	print(array, len);
}
