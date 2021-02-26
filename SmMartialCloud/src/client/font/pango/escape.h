/*
 * Copyright (C) 2018
 */

#ifndef ESCAPE_H
#define ESCAPE_H

namespace font
{
	/*
	 * Escapes the pango markup characters in a text
	 *
	 * The markups escaped are the one used in the pango markup. The special
	 * characters are: @verbatim <>'"& @endverbatim
	 * The escaping is the same as for HTML
	 */
	inline std::string escape_text(const std::string& text)
	{
		std::string result;

		for(auto& c : text)
		{
			switch(c)
			{
				case '&':
					result += "&amp;";
					break;
				case '<':
					result += "&lt;";
					break;
				case '>':
					result += "&gt;";
					break;
				case '\'':
					result += "&apos;";
					break;
				case '"':
					result += "&quot;";
					break;
				default:
					result += c;
			}
		}
		return result;
	}

	/*
	 * Escape only the ampersands. This is used by pango_text to try to 
	 * recover from markup parsing failure.
	 */
	inline std::string semi_escape_text(const std::string& text)
	{
		std::string semi_escaped;

		for(auto& c: text)
		{
			if(c == '&')
				semi_escaped += "&amp;";
			else
				semi_escaped += c;
		}
		return semi_escaped;
	}
}

#endif
