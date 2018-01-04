/*
 * Copyright (C) by Shan Daming <shandaming@hotmail.com>
 */

#ifndef FONT_DESCRIPTOR_H
#define FONT_DESCRIPTOR_H

#include <string>
#include <utility>
#include <vector>

namespace font
{
	/*
	 * Structure used to describe a font, and the subset of the Unicode 
	 * character set it covers.
	 *
	 * used by font_config interface (not specific to sdl_ttf or other)
	 */
	struct Subset_descriptor
	{
		Subset_descriptor() : name(), bold_name(), italic_name(),
		present_codepoints() {}

		explicit Subset_descriptor(const Config& font) : name(font[name])

		std::string name;
		std::string bold_name;
		std::string italic_name;

		typedef std::pair<int, int> Range;
		std::vector<Range> present_codepoints;
	};
}

#endif
