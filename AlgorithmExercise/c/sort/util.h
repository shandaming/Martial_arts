/*
 * Copyright (C) 2020
 */

#ifndef _UTIL_H
#define _UTIL_H

#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof(arr[0]))

void print(const int* array, int len);
void swap(int* left, int* right);
int get_array_max_value(int arr[], int len);

#endif
