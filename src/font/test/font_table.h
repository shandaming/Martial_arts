/*
 * Copyright (C) 2017 by Shan Daming
 */

#ifndef FONT_TABLE_H
#define FONT_TABLE_H

#include <string>
#include "font_cache.h"

class Font_table
{
	public:
		explicit Font_table(const std::string& name, 
				const Font_cache& cache) : table_.emplace(name, cache) {}

		Font_table(const Font_table& table) : 
			table_(table.get_font_table()) {}

		Font_table& operator=(const Font_table& table)
		{
			table_ = table.get_font_table();
		}

		void add(const std::string name, const Font_cache& cache) 
		{
			table_.emplace(name, cache);
		}

		Font_cache get(const std::string name)
		{
			auto it = table_.find(name);
			if(it != table_.end())
				return it->second;

			Font_cache cache(0, nullptr);
			return cache;
		}

		void erase(const std::string name)
		{
			auto it = table_.find(name);
			if(it != table_.end())
				table_.erase(it);
		}

		void clear() { table_.clear(); }

		std::map<std::string, Font> get_font_table() const 
		{
			return table_; 
		}
	private:
		std::map<std::string, Font_cache> table_;
};

#endif
