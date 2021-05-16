/*
   Copyright (C) 2018 by Shan daming
*/

#include <tuple>

#include "point.h"

bool point::operator==(const point& p) const
{
	return x == p.x && y == p.y;
}

bool point::operator!=(const point& p) const
{
	return !operator==(p);
}

bool point::operator<(const point& p) const
{
	return std::tie(x, y) < std::tie(p.x, p.y);
}

bool point::operator<=(const point& p) const
{
	return x < p.x || (x == p.x && y <= p.y);
}

point point::operator+(const point& p) const
{
	return {x + p.x, y + p.y};
}

point& point::operator+=(const point& p)
{
	x += p.x;
	y += p.y;
	return *this;
}

point point::operator-(const point& p) const
{
	return {x - p.x, y - p.y};
}

point& point::operator-=(const point& p)
{
	x -= p.x;
	y -= p.y;
	return *this;
}

std::ostream& operator<<(std::ostream& stream, const point& p)
{
	stream << p.x << ',' << p.y;
	return stream;
}
