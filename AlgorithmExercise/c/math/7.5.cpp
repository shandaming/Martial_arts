/*
 * Copyright (C) 2019
 */

template<typename A, typename N, typename Op>
A power_accumulate_semigroup(A r, A a, N n, Op op)
{
	if(n == 0)
	{
		return r;
	}
	while(1)
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

template<typename A, typename N, typename Op>
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

template<typename A, typename N, typename Op>
A power_monoid(A a, N n, Op op)
{
	if(n == 0)
	{
		return identity_element(op);
	}
	return power_semigroup(a, n, op);
}

template<typename T>
T identity_element(std::plus<T>) { return T(0); }

template<typename T>
T identity_element(std::multiplies<T>) { return T(1); }

template<typename A, typename N, typename Op>
A power_group(A a, N n, Op op)
{
	if(n < 0)
	{
		n = -n;
		a = inverse_operation(op)(a);
	}
	return power_monoid(a, n, op);
}

template<typename T>
std::negate<T> inverse_operation(std::plus<T>) { return std::negate<T>(); }

template<typename T>
reciprocal<T> inverse_operation(std::multiplies<T>) 
{
	return reciprocal<T>();
}

template<typename T>
struct reciprocal
{
	T operator()(const T& x) const
	{
		return T(1) / x;
	}
};
