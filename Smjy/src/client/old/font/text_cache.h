/*
 * Copyright (C) 2017 by Shan Daming
 */

#ifndef TEXT_CACHE_H
#define TEXT_CACHE_H

#include <list>
#include "text_surface.h"

class Text_cache
{
	public:
		Text_cache() {}

		static Text_surface& find(const Text_surface& t);
		static void resize(unsigned int size);
	private:
		typedef std::list<Text_surface> text_list;

		static text_list cache_;
		static unsigned int max_size_;
};

#endif
