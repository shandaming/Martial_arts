/*
 * Copyright (C) 2019
 */

#ifndef TMP_FACTORIAL_H
#define TMP_FACTORIAL_H

template<int N>
struct factorial
{
	enum 
	{
		value = N * factorial<N - 1>::value
	};
};

template<>
struct factorial<1>
{
	enum
	{
		value = 1
	};
};

#endif
