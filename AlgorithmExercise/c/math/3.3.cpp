/*
 * Copyright (C) 2019
 */

#include <algorithm>
#include <vector>

// 素数筛选法

template<typename It, typename N>
void mark_sieve(It first, It last, N factor)
{
	*first = false;
	while(last - first > factor)
	{
		first += factor;
		*first = false;
	}
}

template<typename It, typename N>
void sift0(It first, N n)
{
	std::fill(first, first + n, true);
	N i(0);
	N index_square(3);

	while(index_square < n)
	{
		if(first[i])
		{
			mark_sieve(first + index_square, first + n, i + i + 3);
		}
		++i;
		index_square = 2 * i * (i + 3) + 3;
	}
}

template<typename It, typename N>
void sift1(It first, N n)
{
	It last = first + n;
	std::fill(first, last, true);
	N i(0);
	N index_square(3);
	N factor(3);

	while(index_square < n)
	{
		if(first[i])
		{
			mark_sieve(first + index_square, last, factor);
		}
		++i;
		factor = i + i + 3;
		index_square = 2 * i * (i + 3) + 3;
	}
}

template<typename It, typename N>
void sift(It first, N n)
{
	It last = first + n;
	std::fill(first, last, true);
	N i(0);
	N index_square(3);
	N factor(3);

	while(index_square < n)
	{
		if(first[i])
		{
			mark_sieve(first + index_square, last, factor);
		}
		++i;
		index_square += factor;
		factor += N(2);
		index_square += factor;
	}
}

#include <iostream>

int main(int argc, char* argv[])
{
	std::vector<bool> num0 (32, 0);

	std::vector<uint8_t> num1 {3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23,
	25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47, 49, 51, 53};

	std::vector<uint16_t> num2 {3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 
		23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47, 49, 51, 53};

	std::vector<uint32_t> num3 {3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23,
	25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47, 49, 51, 53};

	std::vector<uint64_t> num4 {3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23,
	25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47, 49, 51, 53};

	sift1(num0.begin(), num0.size());
	//sift1(num1.begin(), num1.size());
	//sift1(num2.begin(), num2.size());
	//sift1(num3.begin(), num3.size());
	//sift1(num4.begin(), num4.size());

	for(auto i : num0)
	{
		std::cout << i << ' ';
	}
	std::cout << "\n";

	return 0;
}
