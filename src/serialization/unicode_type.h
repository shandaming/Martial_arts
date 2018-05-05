#ifndef UNICODE_TYPE_H
#define UNICODE_TYPE_H

#include <vector>
#include <string>
#include <exception>

// unicode 32
namespace ucs4
{
	typedef uint32_t Char;
	typedef std::vector<Char> String;
}

namespace utf8
{
	typedef char Char;
	typedef std::string String;

	/*
	 * Throw by operations encuntering invalid [UTF-8 | UTF-16 | UCS-4] data
	 */
	class Invalid_utf8_exception : public std::exception {};
}

/*
 * For Win32 API
 */
namespace utf16
{
	typedef wchar_t Char;
	typedef std::vector<Char> String;
}

#endif
