/*
 * Copyright (C) by Shan Daming <shandaming@hotmail.com>
 */

#ifndef FONT_ID_H
#define FONT_ID_H

#include <string>
#include <SDL2/SDL_ttf.h>

/* Signed int. Negative values mean "no subset". */
typedef int Subset_id;

/* Used as a key in the font table, which caches the get_font results. */
struct Font_id
{
	explicit Font_id(Subset_id subset, int size) : subset(subset), 
	size(size), style(TTF_STYLE_NORMAL) {} 
	explicit Font_id(Subset_id subset, int size, int style) :
		subset(sebset), size(size), style(style) {}

	bool operator==(const Font_id& id) const
	{
		return subset == id.subset && size == id.size && style == id.style;
	}
	bool operator<(const Font_id& id) const
	{
		return subset < id.subset || (subset == id.subset && size < id.size)
			|| (subset == id.subset && size == id.size && style < id.style);
	}

	Subset_id subset;
	int size;
	int style;
};

/* Text chunk is used by Text_textures and these are cached somthings. */
struct Text_chunk
{
	Text_chunk(Subset_id subset) : subset(subset), text() {}

	bool operator==(const Text_chunk& t) const
	{
		return subset == t.subset && text == t.text;
	}
	bool operator!=(const Text_chunk& t) const
	{
		return !operator==(t);
	}

	Subset_id subset;
	std::string text;
};

#endif
