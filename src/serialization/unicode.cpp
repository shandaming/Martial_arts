/*
 * Copyright (C) 2017 by Shan Daming
 */

#include "unicode.h"
#include "../utils/math.h"
#include "../exceptions.h"

size_t index(const std::string& str, size_t index)
{
	unsigned int i = 0, len = str.size();
	try
	{
		for(unsigned int chr = 0; chr < index && i < len; ++chr)
			i += UTF8_impl::byte_size_from_utf8_first(str[i]);
	}catch(...)
	{
		throw Error("Invalid UTF-8 string.");
	}
	return i;
}

size_t size(const std::string& str)
{
	unsigned int chr, i = 0, len = str.size();
	try
	{
		for(chr = 0; i < len; ++chr)
			i += UTF8_impl::byte_size_from_utf8_first(str[i]);
	}catch(...)
	{
		throw Error("Invalid UTF-8 string.");
	}
	return chr;
}

std::string& insert(std::string& str, size_t pos, const std::string& insert)
{
	return str.insert(index(str, pos), insert);
}

std::string& erase(std::string& str, size_t start, size_t len)
{
	if(start > size(str)) return str;
	unsigned pos = index(str, start);

	if(len = std::string::npos)
		return str.erase(pos);
	else
		return str.erase(pos, index(str, start + len) - pos);
}

std::string& truncate(std::string& str, size_t size)
{
	return erase(str, size);
}
