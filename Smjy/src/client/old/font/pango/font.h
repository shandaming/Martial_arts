/*
 * Copyright (C) 2018 by Shan Daming
 */

#ifndef FONT_H
#define FONT_H

#include <pango/pango.h>
#include "../text.h"

namespace font
{
	/*
	 * Small helper class to make sure the pango font object is destroyed 
	 * properly.
	 */
	class Pango_font
	{
		public:
			Pango_font(const std::string& name, unsigned size, 
					font::Pango_text::FONT_STYLE style) : 
				font_(pango_font_description_new())
			{
				pango_font_description_set_family(font_, name.c_str());
				pango_font_description_set_size(font_, size * PANGO_SCALE);

				if(style != Pango_text::STYLE_NORMAL)
				{
					if(style & Pango_text::STYLE_ITALIC)
						pango_font_description_set_style(font_, 
								PANGO_STYLE_ITALIC);
					if(style & Pango_text::STYLE_BOLD)
						pango_font_description_set_weight(font_, 
								PANGO_WEIGHT_BOLD);
					if(style & Pango_text::STYLE_LIGHT)
						pango_font_description_set_weight(font_, 
								PANGO_WEIGHT_LIGHT);
					if(style & Pango_text::STYLE_UNDERLINE)
						// Do nothing here, underline is a property of the 
						// layout.
				}
			}

			~Pango_font() { pango_font_description_free(font_); }

			Pango_font(const Pango_font&) = delete;
			Pango_font& operator=(const Pango_font&) = delete;

			PangoFontDescription* get() { return font_; }
		private:
			PangoFontDescription* font_;
	};
}

#endif
