/*
 * Copyright (C) 2018
 */

#ifndef STRING_UTILS_H
#define STRING_UTILS_H

template<typename T, typename... Args>
inline std::string string_format(T&& fmt, Args&&... args)
{
	return sprintf(std::forward<T>(fmt), std::forward<Args>(args)...);
}

#endif
