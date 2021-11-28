/*
 * Copyright (C) 2018 by Shan Daming
 */

#ifndef STREAM_OPS_H
#define STREAM_OPS_H

#include <ostream>
#include <pango/pango.h>

namespace font
{
	inline std::ostream& operator<<(std::ostream& os, \
			const PangoRectangle& rect)
	{
		os << rect.x << ',' << rect.y << " x " << rect.width << ',' <<
			rect.height;
		return os;
	}
}

#endif
