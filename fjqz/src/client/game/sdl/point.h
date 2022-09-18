/*
   Copyright (C) 2018 by Shan daming
*/

#ifndef _SDL2_POINT_H
#define _SDL2_POINT_H

#include <iosfwd>
#include <tuple>
#include <SDL2/SDL.h>

struct point : public SDL_Point
{
	point() : SDL_Point{0, 0} {}
	point(int x, int y) : SDL_Point{x, y} {}
	point(const SDL_Point& p) : SDL_Point{p} {}

	bool operator==(const point& point) const
	{
		return x == point.x && y == point.y;
	}

	bool operator!=(const point& point) const
	{
		return !operator==(point);
	}

	bool operator<(const point& point) const
	{
		return std::tie(x, y) < std::tie(point.x, point.y);
	}

	bool operator<=(const point& point) const
	{
		return x < point.x || (x == point.x && y <= point.y);
	}

	point operator+(const point& point) const
	{
		return {x + point.x, y + point.y};
	}

	point& operator+=(const point& point)
	{
		x += point.x;
		y += point.y;
		return *this;
	}

	point operator-(const point& point) const
	{
		return {x - point.x, y - point.y};
	}

	point& operator-=(const point& point)
	{
		x -= point.x;
		y -= point.y;
		return *this;
	}

	point operator*(int s) const
	{
		return {x * s, y * s};
	}

	point& operator*=(int s)
	{
		x *= s;
		y *= s;
		return *this;
	}

	point operator/(int s) const
	{
		return {x / s, y / s};
	}

	point& operator/=(int s)
	{
		x /= s;
		y /= s;
		return *this;
	}
};

std::ostream& operator<<(std::ostream& stream, const point& p);

#endif
