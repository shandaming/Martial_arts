/*
 * Copyright (C) 2019
 */

template<typename T>
struct modulo_multiply
{
	modulo_multiply(const T& i) : modulus(i) {}

	T operator()(const T& n, const T& m) const
	{
		return (n * m) % modulus;
	}

	T modulus;
};

bool odd(int n) { return n & 0x1; }
int half(int n) { return n >> 1; }

template<typename A, typename N, typename Op>
A power_accumulate_semigroup(A r, A a, N n, Op op)
{
	if(n == 0)
	{
		return r;
	}
	while(true)
	{
		if(odd(n))
		{
			r = op(r, a);
			if(n == 1)
			{
				return r;
			}
		}
		n = half(n);
		a = op(a, a);
	}
}

template<typename An typename N, typename Op>
A power_semigroup(A a, N n, Op op)
{
	while(!odd(n))
	{
		a = op(a, a);
		n = half(n);
	}
	if(n == 1)
	{
		return a;
	}
	return power_accumulate_semigroup(a, op(a, a), half(n - 1), op);
}

template<typename T>
bool miller_rabin_test(T n, T q, T k, T w)
{
	modulo_multiply<T> mmult(n);
	T x = power_semigroup(w, q, mmult);
	if(x == T(1) || x == n - T(1))
	{
		return true;
	}
	for(T i(1); i < k; ++i)
	{
		x = mmult(x, x);
		if(x == n - T(1))
		{
			return true;
		}
		if(x == T(1))
		{
			return false;
		}
	}
	return false;
}
