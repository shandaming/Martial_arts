/*
 * Copyright (C) 2019
 */

#include <string>
#include <cstdio>
#include "common/define.h"

template<typename... Args>
inline std::string string_format(const std::string& fmt, Args&&... args)
{
	int size = snprintf(nullptr, 0, fmt.c_str(), std::forward<Args>(args)...);
	std::string buffer;
	buffer.reserve(size + 1);
	buffer.resize(size);
	snprintf(&buffer[0], size + 1, fmt.c_str(), std::forward<Args>(args)...);
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
