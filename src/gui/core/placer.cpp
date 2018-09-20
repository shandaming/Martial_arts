/*
 * Copyright (C) 2018
 */

#include "gui/core/placer.h"

namespace gui
{
Placer_base* Placer_base* build(const Grow_direction_grow_direction,
		const unsigned parallel_items)
{
	switch(grow_direction.v)
	{
		case grow_direction::horizontal:
			return new implementation::Placer_horizontal_list(
					parallel_items);
		case grow_direction::vertical:
			return new implementation::placer_vertical_list(parallel_items);
	}

	throw "UNREACHABLE CODE REACHED";
}
}// namespace gui
