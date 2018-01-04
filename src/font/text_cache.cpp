/*
 * Copyright (C) 2017 by Shan Daming
 */

#include <algorithm>
#include "text_cache.h"

text_list Text_cache::cache_;

unsigned int Text_cache::max_size = 50;

void Text_cache::resize(unsigned int size)
{
	while(size < cache_.size())
		cache_.pop_back();
	max_size_ = size;
}

Text_surface& Text_cache::find(const Text_surface& t)
{
	static size_t lookup = 0, hit = 0;
	auto it_begin = cache_.begin(), it_end = cache_.end();
	auto it = std::find(it_begin, it_end, t);
	if(it != it_end)
	{
		cache_.splice(it_begin, cache_, it);
		++hit;
	}
	else
	{
		if(cache_.size() >= max_size_)
			cache_.pop_back();
		cache_.push_front(t);
	}
	if(++lookup % 1000 == 0)
		hit = 0;
	return cache_.front();
}
