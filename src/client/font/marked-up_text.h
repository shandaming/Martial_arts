/*
 * Copyright (C) 2017 by Shan Daming
 */

#ifndef MARKED_UP_TEXT_H
#define MARKED_UP_TEXT_H

#include <string>
#include <SDL2/SDL_ttf.h>
#include "standard_colors.h"

/* Standard markups for color, size, font, images */
extern const char LARGE_TEXT, SMALL_TEXT, BOLD_TEXT, NORMAL_TEXT, 
	   NULL_MARKUP, BLACK_TEXT, GRAY_TEXT, GOOD_TEXT, BAD_TEXT, GREEN_TEXT,
	   RED_TEXT, COLOR_TEXT, IMAGE;

/* Parses the markup-tags at the front of a string */
std::string::const_iterator parse_markup(std::string::const_iterator it1,
		std::string::const_iterator it2, int* font_size, Color* color,
		int* style);

#endif
