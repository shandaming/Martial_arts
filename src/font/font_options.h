/*
 * Copyright (C) 2017 by Shan Daming
 */

#ifndef FONT_OPTIONS_H
#define FONT_OPTIONS_H

#include <string>

namespace font
{
	/*
	 * Font classes for get_font_families()
	 */
	enum class Family_class
	{
		FANG_SONG,
		FANG_ZHENG_YA_HEI
	};

	inline Family_class str_to_family_class(const std::String& str)
	{
		if(str == "fang_song")
			return Family_class::FANG_SONG;
		return Family_class::FANG_ZHENG_YA_HEI;
	}
}

#endif
