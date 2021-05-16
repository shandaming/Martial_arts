#ifndef _POINT_TEST_H
#define _POINT_TEST_H

#include "point.h"

TEST(pointTest, PointValueEqual)
{
	point p1(-100, 100);
	point p2(-100, 100);

	ASSERT_EQ(p1, p2);
}

TEST(pointTest, PointValueNotEqual)
{
	point p1(0, -30);
	point p2(-30, 0);

	ASSERT_NE(p1, p2);
}

TEST(pointTest, PointValueLessThan)
{
	point p1(0, 1);
	point p2(1, 2);

	ASSERT_LT(p1, p2);
}

TEST(pointTest, PointValueLessThanOrEqual)
{
	point p1(1, 2);
	point p2(2, 3);
	point p3(2, 3);

	ASSERT_LE(p1, p2);
	ASSERT_LE(p2, p3);
}

TEST(pointTest, PointValuePlus)
{
	point p1(1, 2);
	point p2(2, 3);

	ASSERT_EQ(p1 + p2, point(3, 5));
	ASSERT_EQ(point(-2, 3) + point(2, -1), point(0, 2));
}

TEST(pointTest, PointValueMinus)
{
	point p1(3, -4);
	point p2(-1, 2);

	ASSERT_EQ(p1 - p2, point(4, -6));
}

#endif
