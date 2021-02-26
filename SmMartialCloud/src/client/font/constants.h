/*
 * Copyright (C) by Shan Daming <shandaming@hotmail.com>
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>

// font sizes, to be made them parameters
constexpr int SIZE_NORMAL = 14;
/* 
 * automatic computation fo other font sizes, to be made a default for
 * theme-provided values
 */
constexpr int SIZE_TINY = 10 * SIZE_NORMAL / 14, 
                     SIZE_SMALL = 12 * SIZE_NORMAL / 14, 
                     SIZE_15 = 15 * SIZE_NORMAL / 14, 
                     SIZE_PLUS = 16 * SIZE_NORMAL / 14, 
	             SIZE_LARGE = 18 * SIZE_NORMAL / 14, 
                     SIZE_TITLE = 20 * SIZE_NORMAL / 14, 
                     SIZE_XLARGE = 24 * SIZE_NORMAL / 14;

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
constexpr size_t max_text_line_width = 4096;

// String constants
const std::string ellipsis = "...",
	        unicode_minus = "-",
		unicode_en_dash = "_",
		unicode_em_dash = "-",
		unicode_figure_dash = "-",
		unicode_multiplication_sign = "x",
		unicode_bullet = ".",
		weapon_numbers_sep = "x",
		weapon_details_sep = "-";

#endif
