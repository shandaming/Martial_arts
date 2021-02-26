/*
 * Copyright (C) 2019
 */

#ifndef TEMPLATE_BASE_TYPE_H
#define TEMPLATE_BASE_TYPE_H

/*
 * 一切都是类型
 */

/*
 * 判断类型是否相等。
 */

template<bool T>
struct bool_type
{};

// 特化true，false.
template<>
struct bool_type<true>
{
	enum { value = true };
	using type = bool_type<true>;
};

template<>
struct bool_type<false>
{
	enum { value = false };
	using type = bool_type<false>;
};

using true_type = bool_type<true>;
using false_type = bool_type<false>;

template<int N>
struct int_type
{
    enum { value = N };
    using type = int_type<N>;
};


template<typename T, typename U>
struct is_equal
{
	using result = false_type;
};

template<typename T>
struct is_equal<T, T>
{
	using result = true_type;
};

// 一切都是函数

#define INT(v) typename int_type<v>::type
#define BOOLEAN(...) typename bool_type< __VA_ARGS__>::type
#define TRUE() typename true_type::type
#define FALSE() typename false_type::type

#define IS_EQUAL(...) is_equal<__VA_ARGS__>::result

// 取值
template<typename T>
struct value
{
	enum { v = T::value };
};
#define VALUE(...) 	value<__VA_ARGS__>::v

#endif
