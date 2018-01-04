/*
 * Copyright (C) 2017 by Shan Daming
 */

#ifndef FONT_CONFIG_H
#define FONT_CONFIG_H

#include "config.h"

namespace font
{
	bool load_font()
	{}

	struct Font_id
	{
		Font_id(int font_size, int font_style = TTF_STYLE_NORMAL) : 
			size(size), style(style);

		int size, style;
	};

	class Manager
	{
		public:
			Manager();

			Manager(const Manager&) = delete;
			Manager& operator=(const Manager&) = delete;

			void add_font(const Font_id& id, const Font& font);

			Font get_font(const Font_id& id) const;

			void erase_font(const Font_id& id);
		private:
			std::map<Font_id, Font> cache_;
			std::string font_name;
	};
}

#endif
