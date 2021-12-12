/*
 * Copyright (C) 2017 by Shan Daming
 */

#include "unicode.h"
#include "utils/math.h"
#include "exceptions.h"
#include "log"

static lg::Log_domain log_engine("engine");
#define ERR_GENERAL LOG_STREAM(lg::err, lg::general())

namespace utf8
{
	utf8::String lowercase(const utf8::String& str)
	{
		if(!str.empty())
		{
			utf8::Iterator iter(str);
			utf8::String res;

			for(; iter != utf8::Iterator::end(str); ++iter)
			{
				ucs4::Char uchar = *iter;
				// If wchar_t is less than 32 bits wide, we cannot apply 
				// tolower() to all codepoints
				if(uchar <= static_cast<ucs4::Char>(std::numeric_limits<wchar_t>::max()))
					uchar = tolower(static_cast<wchar_t>(uchar));
				res += unicode_cast<utf8::String>(uchar);
			}

			res.append(iter.substr().second, str.end());
			return res;
		}
	}

size_t index(const std::string& str, size_t index)
{
	unsigned int i = 0, len = str.size();
	try
	{
		for(unsigned int chr = 0; chr < index && i < len; ++chr)
			i += ucs4_convert_impl::Utf8_impl::byte_size_from_utf8_first(str[i]);
	}catch(Invalid_utf8_exception&)
	{
		ERR_GENERAL << "Invalid UTF-8 string.\n";
	}
	return i;
}

size_t size(const std::string& str)
{
	unsigned int chr, i = 0, len = str.size();
	try
	{
		for(chr = 0; i < len; ++chr)
			i += ucs4_convert_impl::Utf8_impl::byte_size_from_utf8_first(str[i]);
	}catch(Invalid_utf8_exception&)
	{
		ERR_GENERAL << "Invalid UTF-8 string.\n";
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

void truncate_as_ucs4(utf8::String& str, const size_t size)
{
	ucs4::String str = unicode_cast<ucs4::String>(str);
	if(str.size() > size)
	{
		str.resize(size);
		str = unicode_cast<utf8::String>(str);
	}
}
}
