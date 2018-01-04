/*
 * Copyright (C) 2017 by Shan Daming
 */

#include "font_table.h"

Font_table::Font_table(const std::string name, const Font& font) :
	table_.emplace(name, font) {}

Font_table::Font_table(const Font_table& table) : 
	table_(table.get_font_table()) {}

Font_table& Font_table::operator=(const Font_table& table)
{
	table_ = table.get_font_table();
}

void Font_table::add(const std::string& name, const Font& font)
{
	table_.emplace(name, font);
}

Font Font_table::get(const std::string& name)
{
	auto it = table_.find(name);
	if(it != table_.end())
		return it->second;

	Font()
}
