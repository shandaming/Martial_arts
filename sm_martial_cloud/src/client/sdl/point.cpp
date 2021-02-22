/*
   Copyright (C) 2018 by Shan daming
*/

#include "Point.hpp"

Point::operator SDL_Point() const
{
	return {x, y};
}

Point& Point::operator+=(const Point& point)
{
	x += point.x;
	y += point.y;
	return *this;
}

Point& Point::operator-=(const Point& point)
{
	x -= point.x;
	y -= point.y;
	return *this;
}

std::ostream& operator<<(std::ostream& stream, const Point& point)
{
	stream << point.x << ',' << point.y;
	return stream;
}
