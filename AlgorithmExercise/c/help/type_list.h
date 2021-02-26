/*
 * Copyright (C) 2019
 */

#ifndef TLP_TYPE_LIST_H
#define TLP_TYPE_LIST_H

#include "static_assert.h"

template<typename H, typename T>
struct type_elemt
{
	using head = H;
	using tail = T;
};

template<typename Head, typename... Tails>
struct type_list
{
	using result = type_elemt<Head, typename type_list<Tails...>::result>;
};

template<typename H>
struct type_list<H>
{
	using result = type_elemt<H, null_type>;
};


// value_list
template<int Head, int... Tails>
struct value_list
{
	using result = type_elemt<INT(Head), typename value_list<Tails...>::result>;
};

template<int H>
struct value_list<H>
{
	using result = type_elemt<INT(H), null_type>;
};

template<typename T>
struct length;

template<>
struct length<null_type>
{
	using result = INT(0);
};

template<typename Head, typename Tails>
struct length<type_elemt<Head, Tails>>
{
	using result = typename add<int_type<1>, typename length<Tails>::result>::result;
};

template<typename T, typename Index>
struct type_index;

template<int N>
struct type_index<null_type, int_type<N>>
{
	using result = null_type;
};

template<typename H, typename T>
struct type_index<type_elemt<H, T>, int_type<0>>
{
	using result = H;
};

template<typename H, typename T, int N>
struct type_index<type_elemt<H, T>, int_type<N>>
{
	using result = typename type_index<T, int_type<N - 1>>::result;
};

template<typename T, typename U>
struct append;

template<>
struct append<null_type, null_type>
{
	using result = null_type;
};

template<typename T>
struct append<null_type, T>
{
	using result = typename type_list<T>::result;
};

template<typename H, typename T>
struct append<null_type, type_elemt<H, T>>
{
	using result = type_elemt<H, T>;
};

template<typename H, typename T, typename U>
struct append<type_elemt<H, T>, U>
{
	using result = type_elemt<H, typename append<T, U>::result>;
};

template<typename T, template<typename U> class Pred>
struct any;

template<template<typename T> class Pred>
struct any<null_type, Pred>
{
	using result = false_type;
};

template<typename Head, typename Tail, 
	template<typename T> class Pred>
struct any<type_elemt<Head, Tail>, Pred>
{
	using result = typename if_then_else<typename Pred<Head>::result,
		true_type, typename any<Tail, Pred>::result>::result;
};

template<typename T, template<typename U> class Pred> 
struct all;

template<template<typename U> class Pred>
struct all<null_type, Pred>
{
	using result = true_type;
};

template<typename Head, typename Tail, template<typename T> class Pred>
struct all<type_elemt<Head, Tail>, Pred>
{
	using result = typename if_then_else<typename Pred<Head>::result,
		  typename all<Tail, Pred>::result, false_type>::result;
};

template<template<typename T> class Pred>
struct negative_of
{
	template<typename U>
	struct apply
	{
		using result = typename Not<typename Pred<U>::result>::result;
	};
};

template<typename T, template<typename U> class Pred>
struct all
{
	using result = typename Not<typename any<T, negative_of<Pred>::template apply>::result>::result;
};

template<typename T, template<typename U> class Func>
struct map;

template<template<typename T> class Func>
struct map<null_type, Func>
{
	using result = null_type;
};

template<typename H, typename T, template<typename U> class Func>
struct map<type_elemt<H, T>, Func>
{
	using result = type_elemt<typename Func<H>::result,
		  typename map<T, Func>::result>;
};

template<typename T, template<typename U> class unit>
struct scatter;

template<template<typename> class unit>
struct scatter<null_type, unit>{};

template<typename T, template<typename U> class unit>
struct scatter : unit<T>
{};

template<typename H, typename T, template<typename U> class unit>
struct scatter<type_elemt<H, T>, unit> :
 scatter<H, unit>, scatter<T, unit> 
{};

// lambda返回值
template<typename T>
struct lamhda : lambda_traits<decltype(&T::operator())>
{};

template<typename C, typename R, typename... Args>
struct lambda<R (C::*)(Args...) const>
{
	using return_type = R;
	using para_types = typename type_list<Args...>::result;
};

auto f = [](const int* a,  char y) {return *a + y;};
using lambda_type = decltype(f);

lambda(f);

template<typename Trigle, typename Figure>
struct is_triangle;

template<typename P1, typename P2, typename P3, typename Figure>
struct is_triangle<type_elemt<P1, type_elemt<P2, type_elemt<p3, null_type>>>, Figure>
{
private:
	func_forward_2(connected, belong(points(_1, _2), Figure));
	func_forward_3(in_sameline, belong(points(_1, _2, _3), Figure));
public:
	using result = And(connected<P1, P2>,
			And(connected<P2, P3>,
				And(connected<P3, P1>,
					Not(in_sameline<P1, P2, P3>))));
};

#endif
