/*
 * Copyright (C) 2017 by Shan Daming
 */

#ifndef FONT_COLOR_H
#define FONT_COLOR_H

#include "font_property.h"

class Font_color : public Font_property
{
	public:
		Font_color() = default;
		Font_color(int font_color) : font_color_(font_color) {}

		~Font_color() {}

		void add(int font_color) override { font_color_ = font_color; }

		void get() override { return font_color_; }
	private:
		int font_color_;
};

#endif
