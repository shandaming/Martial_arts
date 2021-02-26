/*
 * Copyright (C) 2019
 */

int gcm0(int a, int b)
{
	while(a != b)
	{
		if(a < b)
		{
			a -= b;
		}
		else
		{
			b -= a;
		}
	}
	return a;
}

int gcm1(int a, int b)
{
	while(a != b)
	{
		while(b < a)
		{
			a -= b;
		}
		std::swap(a, b);
	}
	return a;
}

#include <algorithm>

int segment_remainder(int a, int b)
{
	while(b < a)
	{
		a -= b;
	}
	return a;
}

int gcm(int a, int b)
{
	while(a != b)
	{
		a = segment_remainder(a, b);
		std::swap(a, b);
	}
	return a;
}

int fast_segment_remainder(int a, int b)
{
	if(a <= b)
	{
		return a;
	}
	if(a - b <= b)
	{
		return a - b;
	}
	a = fast_segment_remainder(a, b + b);
	if(a <= b)
	{
		return a;
	}
	return a - b;
}

int fast_segment_gcm(int a, int b)
{
	while(a != b)
	{
		a = fast_segment_remainder(a, b + b);
		std::swap(a, b);
	}
	return a;
}
