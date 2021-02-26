/*
   Copyright (C) 2018 by Shan daming
*/

#include <iostream>
#include <tuple>
#include <SDL2/SDL.h>

/** Holds a 2D Point. */
struct Point
{
	Point() : x(0), y(0)
	{}

	Point(const int x_, const int y_) : x(x_), y(y_)
	{}

	Point(const SDL_Point& p) : x(p.x), y(p.y)
	{}

	/** x coordinate. */
	int x;

	/** y coordinate. */
	int y;

	/** Allow implicit conversion to SDL_Point. */
	operator SDL_Point() const;

	bool operator==(const Point& point) const
	{
		return x == point.x && y == point.y;
	}

	bool operator!=(const Point& point) const
	{
		return !operator==(point);
	}

	bool operator<(const Point& point) const
	{
		return std::tie(x, y) < std::tie(point.x, point.y);
	}

	bool operator<=(const Point& point) const
	{
		return x < point.x || (x == point.x && y <= point.y);
	}

	Point operator+(const Point& point) const
	{
		return {x + point.x, y + point.y};
	}

	Point& operator+=(const Point& point);

	Point operator-(const Point& point) const
	{
		return {x - point.x, y - point.y};
	}

	Point& operator-=(const Point& point);
};

std::ostream& operator<<(std::ostream& stream, const Point& point);
