/*
 * Copyright (C) 2017 by Shan Daming
 */

#ifndef UNICODE_H
#define UNICODE_H

#include <string>
#include "ucs4_iterator_base.h"
#include "ucs4_convert_impl.h"

typedef Iterator_base<std::string, Convert_impl<char>::type> iterator;

/*
 * Codepoint index corresponding to the nth character in a UTF-8 string.
 *
 * @return str.length() if there are less than @p index characters.
 */
size_t index(const std::string& str, const size_t index);

/* Length in characters of a UTF-8 string. */
size_t size(const std::string& str);

/* Insert a UTF-8 string at the specified position */
std::string& insert(std::string& str, size_t pos,
		const std::string& insert);

/* Erases a portion of a UTF-8 string. */
std::string& erase(std::string& str, size_t start, 
		size_t len = std::string::npos);

/* Truncates a UTF-8 string to the specified number of characters. 
 * 
 * @note This implementation does not check for valid UTF-8. Don't use it
 * for user input.
 */
std::string& truncate(std::string& str, size_t size);

#endif
