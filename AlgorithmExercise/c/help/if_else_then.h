/*
 * Copyright (C) 2019
 */

#ifndef TEMPLATE_IF_ELSE_THEN_H
#define TEMPLATE_IF_ELSE_THEN_H

#include "base_operation.h"

// IF ELSE THEN

template<typename Condition, typename Then, typename Else>
struct if_then_else
{};

template<typename Then, typename Else>
struct if_then_else<true_type, Then, Else>
{
	using result = Then;
};

template<typename Then, typename Else>
struct if_then_else<false_type, Then, Else>
{
	using result = Else;
};

#define IF(...) typename if_then_else<__VA_ARGS__>::result

template<typename T, typename U>
using compare_type = IF(BOOLEAN((sizeof(T) > sizeof(U))), T, U);

// 判断类型是否可以转换
template<typename T, typename U>
class is_convertible
{

private:
	using yes = char;
	struct no{ char dumy[2]; };

	static yes test(U);
	static no test(...);
	static T self();
public:
	using result = bool_type<(sizeof(test(self())) == sizeof(yes))>;
};

#define IS_CONVERTIBLE(...) typename is_convertible<__VA_ARGS__>::result

#define IS_BOTH_CONVERTIBLE(T, U) AND(is_convertible<T, U>, is_convertible<U, T>)


#define IS_BASE_OF(T, U) \
	AND(IS_CONVERTIBLE(const U*, const T*), \
	AND(NOT(IS_EQUAL(const T*, const void*)), NOT(IS_EQUAL(const T, const U))))

#endif
