/*
 * Copyright (C) 2017 by Shan Daming
 */

#include "font_config.h"

namespace font
{
	Manager::Manager()
	{
		int res;
		if((res = TTF_Init()) == - 1)
		{
		}

		load_font();
	}

	bool Manager::load_font()
	{
		Config cfg;
		font_name = cfg.get_str();
		if(font_name.empty())
			return false;
		return true;
	}

	void Manager::add_font(const Font_id& id, const Font& font)
	{
		cache_.emplace(id, font);
	}

	Font Manager::get_font(const Font_id& id) const
	{
		auto it = cache_.find(id);
		if(it != cache_.end())
		{
			if(it->second)
				TTF_SetFontStyle(it->second, id.style);
			return it->second;
		}

		if(id.size <= 0)
			return nullptr;

		if(id.style & TTF_STYLE_ITALIC)
		{
			if(TTF_Font* f = TTF_OpenFont(font_name, id.size))
			{
				TTF_SetFontStyle(f, id.style);
				cache_.emplace(id, f);
				return get_font(id);
			}
		}

		if(id.style & TTF_STYLE_BOLD)
		{
			if(TTF_Font* f = TTF_OpenFont(font_name, id.size))
			{
				TTF_SetFontStyle(f, id.style);
				cache_.emplace(id, f);
				return get_font(id);
			}
		}

		if(id.style & TTF_STYLE_UNDERLINE)
		{
			if(TTF_Font* f = TTF_OpenFont(font_name, id.size))
			{
				TTF_setFontStyle(f, id.style);
				cache_.emplace(id, f);
				return get_font(id);
			}
		}

		if(id.style& TTF_STYLE_NORMAL)
		{
			if(TTF_Font* f = TTF_OpenFont(font_name, id.size))
			{
				cache_.emplace(id, f);
				return get_font(id);
			}
		}

		return nullptr;
	}

	void Manager::erase_font(const Font_id& id)
	{
		auto it = cache_.find(id);
		if(it != cache_.end())
			cache_.erase(it);
	}
}
