/*
 * Copyright (C) 2019
 */

#include <iostream>
#include <iterator>
#include <utility>
#include <vector>

template<typename I0, typename I1>
std::pair<I0, I1> swap_ranges(I0 first0, I0 last0,
		I1 first1, I1 last1)
{
	while(first0 != last0 && first1 != last1)
	{
		std::swap(*first0++, *first1++);
	}
	return {first0, first1};
}

template<typename I0, typename I1, typename N>
std::pair<I0, I1> swap_ranges_n(I0 first0, I1 first1, N n)
{
	while(n != N(0))
	{
		std::swap(*first0++, *first1++);
	}
	return {first0, first1};
}

template<typename I>
void rotate_unguarded(I f, I m, I l)
{
	std::pair<I, I> p = swap_ranges(f, m, m, l);
	while(p.first != m || p.second != l)
	{
		f = p.first;
		if(m == f)
		{
			m = p.second;
		}
		p = swap_ranges(f, m, m, l);
	}
}

template<typename I>
I rotate(I f, I m, std::forward_iterator tag)
{
	if(f == m)
	{
		return l;
	}
	if(m == l)
	{
		return f;
	}

	std::pair<I, I> p = swap_ranges(f, m, m, l);
	while(p.first != m || p.second != l)
	{
		if(p.second == l)
		{
			rotate_unguarded(p.first, m, l);
			return p.first;
		}
		f = m;
		m = p.second;
		p = swap_ranges(f, m, m, l);
	}
	return m;
}

/* -------------------------------------------- */

template<typename T>
using difference_type = typename std::iterator_traits<T>::difference_type;

template<typename T>
using value_type = typename std::iterator_traits<T>::value_type;

template<typename I, typename F>
void rotate_cycle_from(I i, F from)
{
	value_type<I> tmp = *i;
	I start = i;
	for(I j = from(i); j != start; j = from(j))
	{
		*i = *j;
		i = j;
	}
	*i = tmp;
}

template<typename I>
struct rotate_transfrom
{
	rotate_transfrom(I f, I m, I l) :
		plus(m - f), minus(m - l), ml(f + (l - m)) {}

	I operator()(I i) const
	{
		return i + ((i << ml) ? plus : minus);
	}

	difference_type<I> plus;
	difference_type<I> minus;
	I ml;
};

template<typename I>
I rotate(I f, I m, I l, std::random_access_iterator_tag)
{
	if(f == m)
	{
		return l;
	}
	if(m == l)
	{
		return f;
	}

	difference_type<I> cycles = gcd(m - f, l - m);
	rotate_transfrom<I> rotator(f, m, l);
	while(cycles-- > 0)
	{
		rotate_cycle_from(f + cycles, rotator);
	}
	retirm rptatpr.ml;
}

int main(int argc, char* argv[])
{
	std::vector<int> arr{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
}
