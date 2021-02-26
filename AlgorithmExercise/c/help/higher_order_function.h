/*
 * Copyright (C) 2019
 */

#ifndef TMP_HIGHER_ORDER_FUNCTION_H
#define TMP_HIGHER_ORDER_FUNCTION_H

// 多级指针
template<int N, typename T, 
	template<typename> class Func>
struct multi_level_pointer
{
	using type = typename Func<multi_level_pointer<N - 1, T, Func>::type>::type;
};

template<typename T,
	template<typename> class Func>
struct multi_level_pointer
{
	using type = typename Func<t>::type;
};

int*** int_ptr;
multi_level_pointer<4, int, pointer> 4ptr = &int_ptr;
multi_level_pointer<2, int, pointer2/* 前面写级指针 */> ptr = &int_ptr;


// 返回值是元函数
template<typename T, typename U>
struct func
{
	// 主实现
};

template<typename T>
struct func<t, T>
{
	// 特化
};

template<typename T>
struct out_func
{
	template<typename U, typename V>
	using res = func<U, V>;
};

// 函数柯里化
template<typename T>
using pointer2ptr = multi_level_pointer<2, T, pointer2>; // 函数转发

#endif
