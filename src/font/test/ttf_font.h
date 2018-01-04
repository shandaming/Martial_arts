/*
 * Copyright (C) 2017 by Shan Daming
 */

#ifndef TTF_FONT_H
#define TTF_FONT_H

#include <string>
#include <SDL2/SDL_ttf.h>

namespace font
{
	class Font
	{
		public:
			Font(const std::string& name, int size = SIZE_NORMAL) : 
				size_(size), font_(TTF_OpenFont(name.c_str(), size)) {}

			Font(const TTF_Font* font) : font_(font) {}

			~Font() { if(font_) CloseFont(font_); }

			bool assign(const std::string name, int size)
			{
				if(font_ = TTF_OpenFont(name.c_str(), size_))
					return true;
				return false;
			}

			Font(const Font& font) : font_(font) {}
			Font& operator=(const Font& font) { font_ = font; }

			operator TTF_Font*() { return font_; }
		private:
			TTF_Font* font_;
	};
}

#endif
