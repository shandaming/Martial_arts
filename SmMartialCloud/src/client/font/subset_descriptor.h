/*
 * Copyright (C) 2017 by Shan Daming
 */

#ifndef SUBSET_DESCRIPTOR_H
#define SUBSET_DESCRIPTOR_H

namespace font
{

	/*
	 * sturcture used to describe a font, and the subset of the Unicode 
	 * character set it covers.
	 *
	 * used by font_config interface (not specific to sdl_ttf)
	 */
	struct Subset_descriptor
	{
		Subset_descriptor() : name(), bold_name(), italic_name(), 
			present_codepoints() {}

		explicit Subset_descriptor(const Config& font) : name, bold_name(),
		italic_name(), present_codepoints()


		std::string name;
		std::optional<std::string> bold_name;
		std::optional<std::string> italic_name;

		typedef std::pair<int, int> range;
		std::vector<range> present_codepoints;
	};
}

#endif
