/*
 * Copyright (C) 2017 by Shan Daming
 */

#ifndef FONT_CONFIG_H
#define FONT_CONFIG_H

#include <set>
#include "font_options.h"
#include "sdl_ttf.h"

namespace font
{
	struct Manager
	{
		Manager() {}
		~Manager() {}

		Manager(const Manager&) = delete;
		Manager& operator=(const Manager&) = delete;
	private:
		// Initialize sdl_ttf concurrent with font::Manager lifetime.
		SDL_ttf sdl_ttf_initializer_;
	};

	/*
	 * load_font_config actually searches the game font path for fonts, and
	 * refreshes the set of loads fonts.
	 *
	 * @returns true in case of success.
	 */
	bool load_font_config();

	/* Returns the currently defined fonts. */
	const std::string& get_font_families();

	/* Test if a font file exists */
	bool check_font_file(const std::string& name);
}

#endif
