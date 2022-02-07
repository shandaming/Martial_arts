#ifndef __STRING_FORMAT_H
#define __STRING_FORMAT_H

#include "fmt/printf.h"

template<typename Format, typename... Args>
inline std::string string_format(Format&& fmt, Args&&... args)
{
        try
        {
            return fmt::sprintf(std::forward<Format>(fmt), std::forward<Args>(args)...);
        }
        catch (const fmt::format_error& formatError)
        {
            std::string error = "An error occurred formatting string \"" + std::string(fmt) + "\" : " + std::string(formatError.what());
            return error;
        }
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
