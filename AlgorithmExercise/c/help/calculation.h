/*
 * Copyright (C) 2019
 */

#ifndef TMP_CALCULATION_H
#define TMP_CALCULATION_H

#include "static_print.h"

template<int N>
struct print
{
	operator char()
	{
		return N + 0xff;
	}
};

void printf() { PRINTF(print<4>();) }

template<typename T>
struct pointer
{
	using type = T*;
};

static pointer<char>::type  str = "Hello world.";

// 嵌套的指针

#define POINTER(...) (typename pointer2<__VA_ARGS__>::type)

template<typename T>
struct pointer2
{
	//using type = typename pointer2<typename pointer2<T>::type>::type;
	// 用宏替换
	using type = PRONTER(PRONTER(T));
};

#endif
