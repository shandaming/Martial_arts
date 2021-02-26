/*
 * Copyright (C) 2019
 */

#ifndef TMP_STATIC_ASSERT_H
#define TMP_STATIC_ASSERT_H

#include "if_else_then.h"

// 递归

// 可变参数模板任意个数值求和
template<typename... N>
struct sum{};

template<typename N, typename... M>
struct sum<N, M...>
{
	using result = typename add<N, typename sum<M...>::result>::type;
};

template<>
struct sum<>
{
	using result = int_type<0>;
};

#define SUM(...) typename sum<__VA_ARGS__>::result

// 利用惰性
// 工厂函数静态选择
template<typename T>
struct clonable_creator
{
	static T* create(const T& instance)
	{
		return instance.clone();
	}
};

template<typename T>
struct unclone_creator
{
	static T* create(const T& instance)
	{
		return new T(instance);
	}
};

template<typename T, bool clonable>
using creator = IF(BOOLEAN(clonable), clonable_creator<T>, unclone_creator<T>);


// static_assert

#define ASSERT_TRUE(T) \
	static_assert(VALUE(T), "Assert Failed. expect "#T" be true, but be false!")

#define ASSERT_FALSE(T) \
	static_assert(!VALUE(T), "Assert Failed. expect "#T" be false, but be true!")

#define ASSERT_EQUAL(T, Expected) \
	static_assert(VALUE(IS_EQUAL(T, Expected)), "Assert Failed. expect "#T" be equal to "#Expected"!")

#define ASSERT_UNEQUAL(T, Expected) \
	static_assert(!VALUE(IS_EQUAL(T, Expected)), "Assert Failed. expect "#T" be not equal to "#Expected"!")


#define ASSERT_EQ(T, Expected) \
	static_assert(VALUE(is_equal<T, Expected>::result), "Assert Failed. expect "#T" be equal to "#Expected"!")

struct null_type;
#define null() null_type;

template<typename T>
struct valid
{
	using result = TRUE();
};

template<>
struct valid<null_type>
{
	using result = FALSE();
};

#define VALID(...) typename valid<__VA_ARGS__>::result;

#define ASSERT_VALID(T) \
	static_assert(VALUE(VALID(T)), "Assert Failed. expect "#T" be valid, but be invalid!");

#define ASSERT_INVALID(T) \
	static_assert(!VALUE(VALID(T)), "Assert Failed. expect "#T" be invalid, but be valid!");

#include "static_print.h"

#if 0

// 测试用例
#define TEST(name) struct UNIQUE_NAME(tlp_test)

TEST("operator add on int type")
{
	using num1 = INT(10);
	using num2 = INT(2);
	ASSERT_EQUAL((ADD(num1, num2)), INT(12));
};

#define FIXTURE(name) namespace tlp_fixture_##name

FIXTURE(TestMetaFunctionInFixture)
{
	template<typename T, typename U>
	using larger_type = IF(BOOLEAN((sizeof(T) > sizeof(U))), T, U);

	struct two_bytes_type
	{
		char dumy[2];
	};

	TEST("int should be larger than two bytes")
	{
		ASSERT_EQUAL(larger_type<int, two_bytes_type>::result, int);
	};

	TEST("char should be smaller than two bytes")
	{
		ASSERT_EQUAL((larger_type<char, two_bytes_type>::result), two_bytes_type);
	};
};

#endif

#endif
