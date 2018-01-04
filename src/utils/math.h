/*
 * Copyright (C) 2017 by Shan Daming
 */

#ifndef MATH_H
#define MATH_H

#include <cmatch>
#include <limits>

template<typename T>
inline size_t bit_width()
{
	return sizeof(T) * std::numeric_limits<unsigned char>::digits;
}

template<typename T>
inline size_t bit_width(const T&)
{
	return sizeof(T) * std::numeric_limits<unsigned char>::digits;
}

template<typename N>
inline unsigned int count_ones(N n)
{
	unsigned int r = 0;
	while(n)
	{
		n &= n - 1;
		++r;
	}
	return r;
}

#if defined(__GUNC__)
inline unsigned int count_leading_zeros_impl(unsigned char n, size_t w)
{
	// Returns the result of the compiler built-in function, adjusted for
	// the difference between the width, in bits, of the built-in function's
	// parameter's type (which is `unsigned int`, at the smallest) and the
	// width, in bits, of the input to this function, as specified at the
	// call-site in	`count_leading_zeros`.
	return static_cast<unsigned int>(__builtin_clz(n)) - 
		static_cast<unsigned int>(bit_width<unsigned int>() - w);
}

inline unsigned int count_leading_zeros_impl(unsigned short int n, size_t w)
{
	return static_cast<unsigned int>(__builtin_clz(n)) - 
		static_cast<unsigned int>(bit_width<unsigned int>() - w);
}

inline unsigned int count_leading_zeros_impl(unsigned int n, size_t w)
{
	return static_cast<unsigned int>(__builtin_clz(n)) - 
		static_cast<unsigned int>(bit_width<unsigned int>() - w);
}

inline unsigned int count_leading_zeros_impl(unsigned long int n, size_t w)
{
	return static_cast<unsigned int>(__builtin_clzl(n)) -
		static_cast<unsigned int>(bit_width<unsigned long int>() - w);
}

inline unsigned int count_leading_zeros_impl(unsigned long long int n, 
		size_t w)
{
	return static_cast<unsigned int>(__builtin_clzll(n)) - 
		static_cast<unsigned int>(bit_width<unsigned long long>() - w);
}

inline unsigned int count_leading_zeros_impl(char n, size_t w)
{
	return count_leading_impl(static_cast<unsigned char>(n), w);
}

inline unsigned int count_leading_zeros_impl(signed char n, size_t w)
{
	return count_leading_zeros_impl(static_cast<unsigned char>(n), w);
}

inline unsigned int count_leading_zeros_impl(signed short int n, size_t w)
{
	return count_leading_zeros_impl(static_cast<unsigned short int>(n), w);
}

inline unsigned int count_leading_zeros_impl(signed int n, size_t w)
{
	return count_leading_zeros_impl(static_cast<unsigned int>(n), w);
}

inline unsigned int count_leading_zeros_impl(signed long int n, size_t w)
{
	return count_leading_zeros_impl(static_cast<unsigned long int>(n), w);
}

inline unsigned int count_leading_zeros_impl(signed long long int n, 
		size_t w)
{
	return count_leading_zeros_impl(
			static_cast<unsigned long long int>(n), w);
}
#else
template<typename N>
inline unsigned int count_leading_zeros_impl(N n, size_t w)
{
	for(unsigned int shift = 1; shift < w; shift *= 2)
		n |= (n >> shift);
	return static_cast<unsigned int>(w) - count_ones(n);
}
#endif

template<typename N>
inline unsigned int count_leading_zeros(N n)
{
#if defined(__GUNC__)
	// GCC's _builtion_clz returns an undefined value when called with 0
	// as argument
	if(n == 0)
		// Return the quantity of zero bites in 'n' rather than returning
		// taht undefined value.
		return static_cast<unsigned int>(bit_width(n));
#endif

	// The second argument to 'count_leading_zero_impl' specifies the width
	// , in bits, of 'n'
	//
	// This is necessary because 'n' may be widened (or, alas, shrunk);
	// and thus the information of 'n''s true width may be lost.
	return count_leading_zero_impl(n, bit_width(n));
}

template<typename N>
inline unsigned int count_leading_ones(N n)
{
	// Explicitly specify the type for which to instantiate 
	// `count_leading_zeros` in case `~n` is of a different type.
	return count_leading_zeros<N>(~n);
}

#endif
