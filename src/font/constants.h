/*
 * Copyright (C) by Shan Daming <shandaming@hotmail.com>
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>

// font sizes, to be made them parameters
extern constexpr int SIZE_NORMAL;
/* 
 * automatic computation fo other font sizes, to be made a default for
 * theme-provided values
 */
extern constexpr int SIZE_TINY, SIZE_SMALL, SIZE_15, SIZE_PLUS, 
	   SIZE_LARGE, SIZE_TITLE, SIZE_XLARGE;

/*
 * For arbitray scaling.
 * (Not used in defining the SIZE_* consts because of spurious compiler
 * warnings.)
 */
inline int relative_size(int size)
{
	return SIZE_NORMAL * size / 14;
}

// GUI1 built-in maximum
extern constexpr size_t max_text_line_width;

// String constants
extern constexpr std::string 
	ellipsis, 
	unicode_minus, 
	unicode_en_dash,
	unicode_em_dash, 
	unicode_figure_dash, 
	unicode_multiplication_sign, 
	unciode_bullet,
	weapon_numbers_sep, 
	weapon_details_sep;

#endif
