/*
 * Copyright (C) 2017 by Shan Daming
 */

#ifndef FONT_DESCRIPTION_H
#define FONT_DESCRIPTION_H

#include <optional>
#include <utility>
#include <vector>
#include "../config.h"

struct Subset_descriptor
{
	Subset_descriptor() : 
		name(), hold_name(), italic_name(), present_codepoints() {}

	Subset_descriptor(const Config& font) : name(font["font", "name"]),
											bold_name(),
											italic_name()
	{
		if(!font["font", "bold_name"].empty())
			bold_name = font["font", "bold_name"];
		if(!font["font", "italic_name"].empty())
			italic_name = font["font", "italic_name"];
	}

	std::string name;
	std::optional<std::string> hold_name;
	std::optional<std::string> italic_name;

	typedef std::pair<int, int> range;
	std::vector<range> present_codepoints;
};

#endif
