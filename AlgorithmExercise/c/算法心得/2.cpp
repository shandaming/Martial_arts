/*
 * Copyright (C) 2019
 */

// 判断某无符号数是不是2的幂或0
bool is_0(unsigned n)
{
	return (n & n - 1) == 0;
}

// 找出一个数的下一个比他大而且值为1的位元数和他相同的数
unsigned snoob(unsigned n)
{
	unsigned smallest = n & ~n;
	unsigned ripple = n + smallest;
	unsigned ones = n ^ ripple;
	ones = (ones >> 2) / smallest;
	return ripple | ones;
}

// 统计字组中值为1的位元数
int pop(unsigned n)
{
	n = n - ((n >> 1) & 0x55555555);

}
