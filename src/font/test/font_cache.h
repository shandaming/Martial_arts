/*
 * Copyright (C) 2017 by Shan Daming
 */

#ifndef FONT_CACHE_H
#define FONT_CACHE_H

#include "font.h"

class Font_cache
{
	public:
		Font_cache() = default;

		Font_cache(const Font_cache& cache) : 
			cache_(cache.get_font_cache()) {}

		Font_cache& operator=(const Font_cache& cache)
		{
			cache_ = cache.get_font_cache();
		}

		void add(const Font_id& id, const Font& font)
		{
			cache_.emplace(id, font);
		}

		Font get(const Font_id& id) const 
		{
			auto it = cache_.find(id);
			if(it != cache_.end())
				return it->second;
			return nullptr;
		}

		void erase(const Font_id& id)
		{
			auto it = cache_.find(id);
			if(it != cache_.end())
				cache_.erase(it);
		}

		std::map<Font_id, Font> get_font_cache() const { return cache_; }
	private:
		std::map<Font_id, Font> cache_;
};

#endif
