/*
 * Copyright (C) 2017 by Shan Daming
 */

#ifndef UCS4_CONVERT_IMPL_H
#define UCS4_CONVERT_IMPL_H

#include "unicode_type.h"
#include "utils/math.h"

namespace ucs4_convert_impl
{
        struct Utf8_impl
        {
	        static const char* get_name() { return "UTF-8"; }

			// Calculate the size of an ucs4 byte
	        static size_t byte_size_from_ucs4_codepoint(ucs4::Char ch)
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
			        throw utf8::Invalid_utf8_exception() // Invalid ucs4
	        }

			// calculate the size of an utf8 byte
	        static int byte_size_from_utf8_first(utf8::Char ch)
	        {
		        if(!(ch & 0x80))
			        return 1; // ASCII charater, 1 byte.

		        // first bit set: charater not in ASCII, mutiple bytes
		        // number of set bits at the beginning = bytes per charater
		        // e.g. 11110xxx indicates a 4-byte character
		        int count = count_leading_ones(ch);
		        if(count == 1 || count > 6) // count > 4 after RFC 3629
			        throw utf8::Invalid_utf8_exception(); 

		        return count;
	        }

	        // writes a UCS-4 character to a UTF-8 stream.
	        template<typename Writer>
	        static inline void write(Writer& out, ucs4::Char ch)
	        {
		        size_t count = byte_size_from_ucs4_codepoint(ch);

				// check whether there is still enough space for n 
				// characters
		        assert(out.can_push(count));

		        if(count == 1)
			        out.push(static_cast<uft8::Char>(ch));
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

	        /*
			 * Reads a UCS-4 character from a UTF-8 stream
			 *
			 * @param beg	An iterator pointing to the first charater of a
			 *				UTF-8 sequence to read
			 *
			 * @param end	An iterator pointing to the end of the UTF-8 
			 *				sequence to read
			 */
	        template<typename Iter>
	        static inline ucs4::Char read(Iter& beg, const Iter& end)
	        {
		        assert(beg != end);

		        size_t size = byte_size_from_utf8_first(*beg);

				ucs4::Char current_char = static_cast<unsigned char>(*beg);

		        // Convert the first character
		        if(size != 1)
			        current_char &= 0xff >> (size + 1);

		        // Convert the continuation bytes
		        ++beg;
		        for(size_t i = 1; i < size; ++i, ++beg)
		        {
			        // If the string ends occurs within an UTF-8-sequence, 
					// this is bad
			        if(beg == end)
				        throw utf8::Invalid_utf8_exception();
			        if((*beg & 0xc0) != 0x80)
				        throw utf8::Invalid_utf8_exception();

			        current_char = (current_char << 6) | 
				        (static_cast<unsigned char>(*beg) & 0x3f);
		        }

		        // Check for non-shortest-form encoding
		        // This has been forbidden in Unicode 3.1 for security 
				// reasons
		        if(size > byte_size_from_ucs4_codepoint(current_char))
			        throw utf8::Invalid_exception();
		        return current_char;
	        }
        };

		struct Utf16_impl
		{
			static const char* get_name() { return "utf16"; }

			// Write a UCS-4 charater to a UTF-16 stream.
			template<typename Writer>
			static inline void write(Writer out, ucs4::Char ch)
			{
				ucs4::Char hit17 = 0x10000;

				if(ch < hit17)
				{
					assert(out.can_push(ch));
					out.push(static_cast<utf16::Char>(ch));
				}
				else
				{
					assert(out.can_push(2));
					ucs4::Char char20 = ch - hit17;
					assert(char20 < (1 << 20));

					ucs4::Char lead = 0x800 + (char20 >> 10);
					ucs4::Char trail = 0xDC00 + (char20 & 0x3FF);
					assert(lead < hit17 && trail < bit17);

					out.push(static_cast<utf16::Char>(lead));
					out.push(static_cast<utf16::Char>(trail));
				}
			}
			
			// Read a UCS4 character from a UTF16 stream
			template<typename Iter>
			static inline ucs4::Char read(Iter& beg, const Iter& end)
			{
				ucs4::Char last10 = 0x3FF;
				ucs4::Char type_filter = 0xFC00;
				ucs4::Char type_lead = 0xD800;
				ucs4::Char type_trail = 0xDC00;

				assert(beg != end);
				ucs4::Char current_char = static_cast<utf16::Char>(*beg);
				++beg;
				ucs4::Char type = current_char & type_filter;
				if(type == type_trail)
					// found trail without head
					throw utf8::Invalid_utf8_exception();
				else if(type == type_lead)
				{
					if(beg == end)
						// If the strin ends occurs within an UTF-16-sequence, this is bad
						throw utf8::Invalid_utf8_exception();
					if((*beg & type_filter) != type_trail)
						throw utf8::Invalid_utf8_exception();

					current_char &= last10;
					current_char <<= 10;
					current_char += (*beg & last10);
					current_char += 0x10000;
					++beg;
				}
				return current_char;
			}
		};

		struct Utf32_impl
		{
			static const char* get_name() { return "UCS-4"; }

			// Write a UCS-4 charater to UCS-4 stream
			template<typename Writer>
			static inline void write(Write out, ucs4::Char ch)
			{
				assert(out.can_push(1));
				out.push(ch);
			}

			// Read an UCS-4 charater from a UCS-4 stream
			template<typename Iter>
			static inline ucs4::Char read(Iter& beg, const Iter& end)
			{
				assert(beg != end);
				ucs4::Char current_char = *beg;
				++beg;
				return current_char;
			}
		};

        template<typename Char>
        struct Convert_impl {};

        template<>
        struct Convert_impl<utf8::Char>
        {
	        typedef Utf8_impl Type;
        };

		template<>
		struct Convert_impl<utf16::Char>
		{
			typedef Utf16_impl Type;
		};

		template<>
		struct Convert_impl<ucs4::Char>
		{
			typedef Utf32_impl Type;
		};
}

#endif
