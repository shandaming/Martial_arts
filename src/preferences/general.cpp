/*
 * Copyright (C) 207 by Shan Daming
 */

#include "general.h"

extern Config prefs;

/*
 * Stores all the static, default values for certain game preferences. The
 * values are kept here foor easy modification without a lengthy rebuild.
 *
 * Add any variables of similar type here.
 */
const int min_font_scaling = 80;
const int max_font_scaling = 150;

int font_scaling()
{
	// Clip at 80 because if it's too low it'll cause crashes.
	return std::max<int>(
			std::min<int>(prefs.get_integer({"font", "scale"}),
				max_font_scaling), min_font_scaling);
}

void set_font_scaling(int scale)
{}

int font_scaled(int size)
{
	return size * font_scaling() / 100;
}
