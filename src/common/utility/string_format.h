/*
 * Copyright (C) 2019
 */

#ifndef __STRING_FORMAT_H
#define __STRING_FORMAT_H

#include <string>
#include <cstdio>

#include "define.h"

template<typename... Args>
inline std::string string_format(const char* fmt, Args&&... args)
{
	int size = snprintf(nullptr, 0, fmt, std::forward<Args>(args)...);
	std::string buffer;
	buffer.reserve(size + 1);
	buffer.resize(size);
	snprintf(&buffer[0], size + 1, fmt, std::forward<Args>(args)...);
	return buffer;
};

inline bool is_format_empty_or_null(const char* fmt)
{
	return fmt == nullptr;
}

inline bool is_format_empty_or_null(const std::string& fmt)
{
	return fmt.empty();
}

#endif
