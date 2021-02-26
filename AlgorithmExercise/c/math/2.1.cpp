/*
 * Copyright (C) 2019
 */

// 多次加这数
int multiply0(int n, int a)
{
	if(n == 1)
	{
		return a;
	}
	return multiply0(n - 1, a) + a;
}

// 埃及乘法

template<typename N>
int half(N n) { return n >> 1; }

template<typename N>
int odd(N n) { return bool(n & 0x1); } // 判断是否是奇数

int multiply1(int n, int a)
{
	if(n == 1)
	{
		return a;
	}
	int res = multiply1(half(n), a + a);
	if(odd(n))
	{
		res += a;
	}
	return res;
}

// 改进

// 乘法累加函数
int mult_acc0(int r, int n, int a)
{
	if(n == 1)
	{
		return r + a;
	}
	if(odd(n))
	{
		return mult_acc0(r + a, half(n), a + a);
	}
	else
	{
		return mult_acc0(r, half(n), a + a);
	}
}

int mult_acc1(int r, int n, int a)
{
	if(n == 1)
	{
		return r + a;
	}
	if(odd(n))
	{
		r += a;
	}
	return mult_acc1(r, half(n), a + a);
}

int mult_acc2(int r, int n, int a)
{
	if(odd(n))
	{
		r = r + a;
		if(n == 1)
		{
			return r;
		}
	}
	return mult_acc2(r, half(n), a + a);
}

int mult_acc3(int r, int n, int a)
{
	if(odd(n))
	{
		r += a;
		if(n == 1)
		{
			return r;
		}
	}
	n = half(n);
	a += a;
	return mult_acc3(r, n, a);
}

// 优化好的乘法累加函数
template<typename A, typename N>
int mult_acc4(A r, N n, A a)
{
	if(n == 0)
	{
		return r;
	}
	while(1)
	{
		if(odd(n))
		{
			r += a;
			if(n == 1)
			{
				return r;
			}
		}
		n = half(n);
		a += a;
	}
}

// 乘法函数
int multiply2(int n, int a)
{
	if(n == 1)
	{
		return a;
	}
	return mult_acc4(a, n - 1, a);
}

int multiply3(int n, int a)
{
	while(!odd(n))
	{
		a += a;
		n = half(n);
	}
	if(n == 1)
	{
		return a;
	}
	return mult_acc4(a, n - 1, a);
}

template<typename A, typename N>
int multiply4(N n, A a)
{
	while(!odd(n))
	{
		a += a;
		n = half(n);
	}
	if(n == 1)
	{
		return a;
	}
	return mult_acc4(a, half(n - 1), a + a);
}

#include <cstdio>
#include <cstdlib>

int main(int argc, char* argv[])
{
	if(argc != 3)
	{
		fprintf(stderr, "Use:  n a\n");
		return 0;
	}

	int n = atoi(argv[1]);
	int a = atoi(argv[2]);

	printf("multiply0 = %d\n"
			"multiply1 = %d\n"
			"multiply2 = %d\n"
			"multiply3 = %d\n"
			"multiply4 = %d\n",
			multiply0(n, a), multiply1(n, a), multiply2(n, a),
			multiply3(n, a), multiply4(n, a));

	return 0;
}
