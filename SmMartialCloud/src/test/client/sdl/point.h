/*
   Copyright (C) 2018 by Shan daming
*/

#ifndef _SDL2_POINT_H
#define _SDL2_POINT_H

#include <ostream>

struct point
{
	point() : x(0), y(0) {}
	point(const int x_, const int y_) : x(x_), y(y_) {}

	bool operator==(const point& p) const;
	bool operator!=(const point& p) const;
	bool operator<(const point& p) const;
	bool operator<=(const point& p) const;
	point operator+(const point& p) const;
	point& operator+=(const point& p);
	point operator-(const point& p) const;
	point& operator-=(const point& p);

	int x;
	int y;
};

std::ostream& operator<<(std::ostream& stream, const point& p);

#endif
