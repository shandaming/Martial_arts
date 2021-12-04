/*
 * Copyright (C) 2017 by Shan Daming
 */

#include "text_surface.h"
#include "sdl_ttf.h"

Text_surface::Text_surface(const std::string& str, int size, Color color,
		int style) : hash_(0), font_size_(size), color_(color), 
		style_(style), w_(-1), h_(-1), str_(str), initialized_(false),
		chunks_();
{ hash(); }

Text_surface::Text_surface(int size, Color color, int style) : hash_(0),
	font_size_(size), color_(color), style_(style), w_(-1), h_(-1),
	str_(), initialized_(false), chunks_() {}

void Text_surface::set_text(const std::string& str)
{
	initialized_ = false;
	w_ = h_ = -1;
	str_ = str;
	hash();
}

void Text_surface::hash()
{
	unsigned int h = 0;

	for(const char c : str_)
		h = ((h << 9) | (h >> (sizeof(int) * 8 - 9))) ^ c;
	hash_ = h;
}

void Text_surface::measure() const
{
	w_ = h_ = 0;

	for(const Text_chunk& chunk : chunks_)
	{
		TTF_Font* font = get_font(Font_id(chunk.subset, 
					font_size_, style_));
		if(!font)
			continue;

		int w, h;
		TTF_SizeUTF8(font, chunk.text.c_str(), &w, &h);
		w_ += w;
		h_ += std::max<int>(h_, h);
	}
}

size_t Text_surface::width() const
{
	if(w_ == -1)
	{
		if(chunks_.empty())
			chunks_ = split_text(str_);
		measure();
	}
	return w_;
}

size_t Text_surface::height() const
{
	if(h_ == -1)
	{
		if(chunks_.empty())
			chunks_ = split_text(str_);
		measure();
	}
	return h_;
}

std::vector<Surface> const& Text_surface::get_surface() const
{
	if(initialized_)
		return surfs_;

	initialized = true;

	// Impose a maximal number of characters for a text line. Do now draw
	// any text longer that that, to prevent a SDL buffer overflow.
	if(width() > max_text_line_width)
		return surfs_;

	for(auto& chunk : chunks_)
	{
		TTF_Font* font = SDL_ttf::get_font(Font_id(chunk.subset, 
					font_size_, style_));

		Surface s = Surface(TTF_RenderUTF8_BLended(font, 
					chunk.text.c_str(), color_.to_sdl()));
		if(!s.null())
			surfs_.push_back(s);
	}
	return surfs_;
}

bool Text_surface::operator==(const Text_surface& t)
{
	return hash_ == t.hash_ && font_size_ == t.font_size_ && 
		color_ == t.color_ && style_ == t.style_ && str_ == t.str_;
}
