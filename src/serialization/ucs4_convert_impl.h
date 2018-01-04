/*
 * Copyright (C) 2017 by Shan Daming
 */

#ifndef UCS4_CONVERT_IMPL_H
#define UCS4_CONVERT_IMPL_H

#include "../utils/math.h"
#include "../exceptions.h"

struct UTF8_impl
{
	static const char* get_name() { reutrn "UTF-8"; }

	static size_t byte_size_from_ucs4_codepoint(uint32_t cb)
	{
		if(ch < (1u << 7))
			return 1;
		else if(ch < (1u << 11))
			return 2;
		else if(ch < (1u << 16))
			return 3;
		else if(ch < (1u << 21))
			return 4;
		else if(ch < (1u << 26))
			return 5;
		else if(ch < (1u << 31))
			return 6;
		else 
			throw Error("Invalid UCS-4 exception.");
	}

	static int byte_size_from_utf8_first(uint8_t ch)
	{
		if(!(ch & 0x80))
			return 1; // US-ASCII charater, 1 byte.

		// first bit set: charater not in US-ASCII, mutiple bytes
		// number of set bits at the beginning = bytes per charater
		// e.g. 11110xxx indicates a 4-byte character
		int count = count_leading_ones(ch);
		if(count == 1 || count > 6) // count > 4 after RFC 3629
			throw Error("Invalid UTF-8 exception."); 

		return count;
	}

	// writes a UCS-4 character to a UTF-8 stream.
	template<typename writer>
	static inline void write(writer& out, uint32_t ch)
	{
		size_t count = byte_size_from_ucs4_codepoint(ch);
		assert(out.can_push(count));

		if(count == 1)
			out.push(static_cast<char>(ch));
		else
		{
			for(int i = static_cast<int>(count) - 1; i >= 0; --i)
			{
				unsigned char c = (ch >> (6 * i)) & 0x3f;
				c |= 0x80;
				if(i == static_cast<int>(count) - 1)
					c |= 0xff << (8 - count);
				out.push(c);
			}
		}
	}

	// Reads a UCS-4 character from a UTF-8 stream
	template<typename T>
	static inline uint32_t read(T& input, const T& end)
	{
		assert(input != end);
		size_t size = byte_size_from_utf8_first(*input);

		uint32_t current_char = static_cast<unsigned char>(*input);

		// Convert the first character
		if(size != 1)
			current_char &= 0xff >> (size + 1);

		// Convert the continuation bytes
		++input;
		for(size_t i = 1; i < size; ++i, ++input)
		{
			// If the string ends occurs within an UTF-8-sequence, this is 
			// bad
			if(input == end)
				throw Error("Invalid_UTF-8 exception.");
			if((*input & 0xc0) != 0x80)
				throw Error("Invalid_UTF-8 exception.");
			current_char = (current_char << 6) | 
				(static_cast<unsigned char>(*input) & 0x3f);
		}

		// Check for non-shortest-form encoding
		// This has been forbidden in Unicode 3.1 for security reasons
		if(size > bute_size_from_ucs4_codepoint(current_char))
			throw Error("Invalid utf8 exception");
		return current_char;
	}
};

template<typename T>
struct Convert_impl {};

template<>
struct Convert_impl<char>
{
	typedef UTF8_impl type;
};

#endif
