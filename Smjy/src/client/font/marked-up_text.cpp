/*
 * Copyright (C) 2017 by Shan Daming
 */

#include "marked-up_text.h"

/*
 * NOTE: if you add more markup characters below, you'll need to update
 * the list in campaign_server.cpp (illegal_markup_chars) to blacklist
 * them for add-on names and titles.
 */
const char LARGE_TEXT = '*', SMALL_TEXT = '`', BOLD_TEXT = '~',
	  NORMAL_TEXT = '{', NULL_MARKUP = '^', BLACK_TEXT = '}', 
	  GRAY_TEXT = '|', GOOD_TEXT = '@', BAD_TEXT = '#', GREEN_TEXT = '@',
	  RED_TEXT = '#', COLOR_TEXT = '<', IMAGE = '&';

std::string::const_iterator parse_markup(std::string::const_iterator it1,
		std::string::const_iterator it2, int* font_size, Color* color,
		int* style)
{
	while(it1 != it2)
	{
		switch(*it1)
		{
			case '\\':
				// This must either be a quoted special character or a 
				// quoted backslash - either way, remove leading backslash
				break;
			case BAD_TEXT:
				if(color) *color = BAD_COLOR;
				break;
			case GOOD_TEXT:
				if(color) *color = GOOD_COLOR;
				break;
			case NORMAL_TEXT:
				if(color) *color = NORMAL_COLOR;
				break;
			case BLACK_TEXT:
				if(color) *color = BLACK_COLOR;
				break;
			case GRAY_TEXT:
				if(color) *color = GRAY_COLOR;
				break;
			case LARGE_TEXT:
				if(font_size) *font_size += 2;
				break;
			case SMALL_TEXT:
				if(font_size) *font_size -= 2;
				break;
			case BOLD_TEXT:
				if(style) *style |= TTF_STYLE_BOLD;
				break;
			case NULL_MARKUP:
				return it1 + 1;
			case COLOR_TEXT:{
				auto start = it1;
				// Very primitive parsing for rgb value should look like
				// <213, 14, 151>
				++it1;
				uint8_t red = 0, green = 0, blue = 0, temp = 0;
				while(it1 != it2 && *it1 >= '0' && *it1 <= '9')
				{
					temp *= 10;
					temp += static_cast<int>(*it1);
					++it1;
				}
				red = temp;
				temp = 0;
				if(it1 != it2 && *it1 == ',')
				{
					++it1;
					while(it1 != it2 && *it1 >= '0' && *it1 <= '9')
					{
						temp *= 10;
						temp += static_cast<int>(*it1);
						++it1;
					}
					green = temp;
					temp = 0;
				}
				if(it1 != it2 && *it1 == ',')
				{
					++it1;
					while(it1 != it2 && *it1 >= '0' && *it1 <= '9')
					{
						temp *= 10;
						temp += static_cast<int>(*it1);
						++it1;
					}
				}
				blue = temp;
				if(it1 != it2 && *it1 == '>')
				{
					Color temp_color{red, green, blue, 0};
					if(color) *color = temp_color;
				}
				else
					// stop parsing and do not consume any chars
					return start;
				if(it1 == it2) return it1;
				break;}
			default:
				return it1;
                                break;
		}
		++it1;
	}
	return it1;
}
