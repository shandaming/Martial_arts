/*
 * Copyright (C) 2019
 */

//  % /

#include <cstdio>

int fast_segment_remainder1(int a, int b)
{
	printf("a = %d, b = %d\n", a, b);
	if(a < b)
	{
		return a;
	}
	if(a - b < b)
	{
		return a - b;
	}
	a = fast_segment_remainder1(a, b + b);
	printf("a = %d, b = %d\n", a, b);
	if(a < b)
	{
		return a;
	}
	return a - b;
}

int largest_doubling(int a, int b)
{
	while(a - b >= b)
	{
		b += b;
	}
	return b;
}

int half(int v)
{
	return v >> 1;
}

int remainder(int a, int b)
{
	printf("a = %d, b = %d\n", a, b);
	if(a < b)
	{
		return a;
	}
	int c = largest_doubling(a, b);
	a -= c;
	while(c != b)
	{
		printf("a = %d, b = %d\n", a, b);
		c = half(c);
		if(c <= a)
		{
			a -= c;
		}
	}
	return a;
}

int quotient(int a, int b)
{
	printf("a = %d, b = %d\n", a, b);
	if(a < b)
	{
		return 0;
	}
	int c = largest_doubling(a, b);
	int n = 1;
	a -= c;
	while(c != b)
	{
		printf("a = %d, b = %d\n", a, b);
		c = half(c);
		n += n;
		if(c <= a)
		{
			a -= c;
			++n;
		}
	}
	return n;
}

template<typename T, typename U>
T quotinet_remainder(U a, U b)
{
	if(a < b)
	{
		return T(0);
	}
	U c = largest_doubling(a, b);
	T n(1);
	a -= c;
	while(c != b)
	{
		c = half(c);
		n += n;
		if(c <= a)
		{
			a -= c;
			++n;
		}
	}
	return {n, a};
}

int remainder_fibonacci(int a, int b)
{
	if(a < b)
	{
		return a;
	}
	int c = b;
	do
	{
		int tmp = c;
		c += b;
		b = tmp;
	}while(a >= c);

	do
	{
		if(a >= b)
		{
			a -= b;
		}
		int tmp = c - b;
		c = b;
		b = tmp;
	}while(b < c);
	return a;
}

#include <algorithm>

int gcm_remainder(int a, int b)
{
	while(b != 0)
	{
		a = remainder(a, b);
		std::swap(a, b);
	}
	return a;
}

int gcd(int a, int b)
{
	while(b != 0)
	{
		a %= b;
		std::swap(a, b);
	}
	return a;
}

int gcd1(int a, int b)
{
	if(b == 0)
		return a;
	
	a %= b;
	std::swap(a, b);
	return gcd(a, b);
}

template<int A, int B>
struct gcd2
{
	enum { result = gcd2<B, A % B>::result};
};

template<int A>
struct gcd2<A, 0>
{
	enum { result = A };
};

int main(int argc, char* argv[])
{
	int a = 45;
	int b = 6;

	printf("fast_segment_remainder1 = %d"
			", remainder = %d"
			", quotient = %d\n",
			fast_segment_remainder1(a, b),
			remainder(a, b),
			quotient(a, b));

	printf("-----------------------------------\n");
	int res = gcd2<45, 6>::result;
	printf("result = %d\n", res);
}
