#ifndef LOGGING_LOG_H
#define LOGGING_LOG_H

#include "string_format.h"

enum log_level : uint8_t
{
	LOG_LEVEL_DISABLED = 0,
	LOG_LEVEL_TRACE,
	LOG_LEVEL_DEBUG,
	LOG_LEVEL_INFO,
	LOG_LEVEL_WARN,
	LOG_LEVEL_ERROR,
	LOG_LEVEL_FATAL,

	NUM_ENABLED_LOG_LEVELS
};

template<typename Fmt, typename... Args>
inline void out_message(std::string_view filter, log_level level, Fmt&& fmt, Args&&... args)
{
	out_message(filter, level, string_format(std::forward<Fmt>(fmt), std::forward<Args>(args)...));
}

void out_message(std::string_view filter, log_level level, std::string&& message);

#define LOG_MESSAGE_BODY(filter_type, level, ...)	\
	do	\
	{	\
        out_message(filter_type, level, __VA_ARGS__);   \
	}while(0)

#define LOG_TRACE(filter_type, ...)	\
	LOG_MESSAGE_BODY(filter_type, LOG_LEVEL_TRACE, __VA_ARGS__)

#define LOG_DEBUG(filter_type, ...)	\
	LOG_MESSAGE_BODY(filter_type, LOG_LEVEL_DEBUG, __VA_ARGS__)

#define LOG_INFO(filter_type, ...)	\
	LOG_MESSAGE_BODY(filter_type, LOG_LEVEL_INFO, __VA_ARGS__)

#define LOG_WARN(filter_type, ...)	\
	LOG_MESSAGE_BODY(filter_type, LOG_LEVEL_WARN, __VA_ARGS__)

#define LOG_ERROR(filter_type, ...)	\
	LOG_MESSAGE_BODY(filter_type, LOG_LEVEL_ERROR, __VA_ARGS__)

#define LOG_FATAL(filter_type, ...)	\
	LOG_MESSAGE_BODY(filter_type, LOG_LEVEL_FATAL, __VA_ARGS__)

#endif
