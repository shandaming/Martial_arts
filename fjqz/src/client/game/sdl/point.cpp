/*
   Copyright (C) 2018 by Shan daming
*/

#include <ostream>
#include "point.h"

std::ostream& operator<<(std::ostream& stream, const point& point)
{
	stream << '(' << point.x << ',' << point.y << ')';
	return stream;
}
