/*
 * Copyright (C) 2017 by Shan Daming
 */

#ifndef TEXT_SURFACE_H
#define TEXT_SURFACE_H

#include <SDL2/SDL_ttf.h>
#include <string>
#include "font_id.h"
#include "../sdl/surface.h"

class Text_surface
{
	public:
		Text_surface(const std::string& str, int size, 
				Color color, int style);

		Text_surface(int size, Color color, int style);

		void set_text(const std::string& text);

		void measure() const;

		size_t width() const;
		size_t height() const;

		
		std::vector<Surface> const& get_surface() const;

		bool operator==(const Text_surface& t) const;
		bool operator!=(const Text_surface& t) const 
		{ return !operator(t); }
	private:
		void hash();
		int hash_;

		int font_size_;
		Color color_;
		int style_;

		mutable int w_, h_;
		std::string str_;

		mutable bool initialized_;
		mutable std::vector<Text_chunk> chunks_;
		mutable std::vector<surface> surfs_;
};

#endif
