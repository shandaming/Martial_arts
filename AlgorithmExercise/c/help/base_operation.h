/*
 * Copyright (C) 2019
 */

#ifndef TEMPLATE_BASE_OPERATION_H
#define TEMPLATE_BASE_OPERATION_H

#include "base_type.h"

template<typename N1, typename N2>
struct add
{};

// +
template<int N1, int N2>
struct add<int_type<N1>, int_type<N2>>
{
	using type = int_type<N1 + N2>;
};
#define ADD(...) typename add<__VA_ARGS__>::type

// ++
template<typename T>
struct inc
{};

template<int N>
struct inc<int_type<N>>
{
	using type = int_type<N + 1>;
};
#define INC(v) typename inc<v>::type

// -
template<typename T1, typename T2>
struct sub
{};

template<int N1, int N2>
struct sub<int_type<N1>, int_type<N2>>
{
	using type = int_type<N1 - N2>;
};
#define SUB(...) typename sub<__VA_ARGS__>::type

// --
template<typename T>
struct dec
{};

template<int N>
struct dec<int_type<N>>
{
	using type = int_type<N - 1>;
};
#define DEC(v) typename dec<v>::type

// *
template<typename T, typename U>
struct mul
{};

template<int N1, int N2>
struct mul<int_type<N1>, int_type<N2>>
{
	using type = int_type<N1 * N2>;
};
#define MUL(...) typename mul<__VA_ARGS__>::type

// /
template<typename T, typename U>
struct Div
{};

template<int N1, int N2>
struct Div<int_type<N1>, int_type<N2>>
{
	using type = int_type<N1 / N2>;
};
#define DIV(...) typename Div<__VA_ARGS__>::type

// %
template<typename T, typename U>
struct Mod
{};

template<int N1, int N2>
struct Mod<int_type<N1>, int_type<N2>>
{
	using type = int_type<N1 % N2>;
};
#define MOD(...) typename Mod(__VA_ARGS__)::type


// 逻辑

// not
template<typename T>
struct Not
{};

template<bool B>
struct Not<bool_type<B>>
{
	using type = bool_type<!B>;
};
#define NOT(b) typename Not<b>::type

// and
template<typename T, typename U>
struct And
{};

template<bool B, bool U>
struct And<bool_type<B>, bool_type<U>>
{
	using type = bool_type<B and U>;
};
#define AND(...) typename And<__VA_ARGS__>::type

// or
template<typename T, typename U>
struct Or
{};

template<bool B, bool U>
struct Or<bool_type<B>, bool_type<U>>
{
	using type = bool_type<B or U>;
};
#define OR(...) typename Or<__VA_ARGS__>::type

// empty type
struct empty_type
{};

template<>
struct value<empty_type>
{
	enum { v = 0 };
};

#endif
