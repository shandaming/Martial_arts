/*
 * Copyright (C) 2017 by Shan Daming <shandaming@hotmail.com>
 */

#ifndef SDL_TTF_H
#define SDL_TTF_H

#include <SDL2/SDL_ttf.h>
#include "text_cache.h"
#include "constants.h"
#include "marked-up_text.h"
#include "../serialization/unicode.h"
#include "../serialization/string_utils.h"

// Return a SDL surface containing the text rendered in a given color
Surface get_render_text(const std::string& text, int size, 
		const Color& color, int style = 0);

SDL_Rect draw_text_line(Texture& gui_texture, const SDL_Rect& area,
		int size, const Color& color, const std::string& text,
		int x, int y, bool use_tooltips, int style);

TTF_Font* get_font(Font_id& id);

/* 
 * If the text exceeds the specified max width, end it width and ellipsts
 * (...)
 */
std::string make_text_ellipsis(const std::string& text, int font_size, 
		int max_width, int style);

/* Returns the maximum height of a font, in pixels */
int get_max_height(int size);

/*
 * Determine the width of a line of text given a certain font size.
 * The font type used is the default wesnoth font type.
 */
int line_width(const std::string& line, int font_size, int style);

/*
 * Determine the size of a line of text text given a certain font size.
 * Similar to line width, but for both coordinates.
 */
SDL_Rect line_size(const std::string& line, int font_size, int style);


struct SDL_ttf
{
	SDL_ttf();
	~SDL_ttf();

	SDL_ttf(const SDL_ttf&) = delete;
	SDL_ttf& operator=(const SDL_ttf&) = delete;

	// Load a font
	static TTF_Font* get_font(Font_id);

	// Set the list of fonts
	static void set_font_list(const std::vector<font::Subset_descriptor>& fontlist);

	// Split a utf8 string into text_chunks
	static std::vector<Text_chunk> split_text(const std::string& text);
};

#endif
