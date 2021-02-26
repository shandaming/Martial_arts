/*
 * Copyright (C) 2019
 */

int fib0(int n)
{
	if(n == 0)
	{
		return 0;
	}
	if(n == 1)
	{
		return 1;
	}
	return fib0(n - 1) + fib0(n - 2);
}

#include <utility>

std::pair<int, int> fib1(int n)
{
	if(n == 0)
	{
		return {0, 0};
	}
	std::pair<int, int> v {0, 1};
	for(int i = 1; i < n; ++i)
	{
		v = {v.second, v.first + v.second};
	}
	return v;
}

template<int N>
struct fib
{
	enum { result = fib<N - 1>::result + fib<N - 2>::result };
};

template<>
struct fib<0>
{
	enum { result = 0 };
};

template<>
struct fib<1>
{
	enum { result = 1 };
};

#include <iostream>

int main(int argc, char* argv[])
{
	std::cout << " res : " << fib<12>::result << std::endl;
	return 0;
}
