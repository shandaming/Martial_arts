/*
 * Copyright (C) 2018
 */

#include <sstream>
#include "appender.h"
#include "string_utils.h"

namespace lg
{
	Appender::Appender(uint8 id, const std::string& name, 
			Log_level level, Appender_flags flags) : 
		id_(id), name_(name), level_(level), flags_(flags) { }

	void Appender::write(Log_message* message)
	{
	    if(!level_ || level_ > message->level)
		{
			return;
		}

	    std::ostringstream ss;

	    if(flags_ & APPENDER_FLAGS_PREFIX_TIMESTAMP)
		{
			ss << message->get_time_str() << ' ';
		}

	    if(flags_ & APPENDER_FLAGS_PREFIX_LOGLEVEL)
		{
			ss << string_format("%-5s ", 
					Appender::get_log_level_str(message->level));
		}

	    if(flags_ & APPENDER_FLAGS_PREFIX_LOGFILTERTYPE)
		{
			ss << '[' << message->type << "] ";
		}

	    message->prefix = ss.str();
	    write_stream(message);
	}

	const char* Appender::get_log_level_str(Log_level level)
	{
	    switch (level_)
	    {
			case LOG_LEVEL_FATAL:
				return "FATAL";
			case LOG_LEVEL_ERROR:
				return "ERROR";
			case LOG_LEVEL_WARN:
				return "WARN";
			case LOG_LEVEL_INFO:
				return "INFO";
			case LOG_LEVEL_DEBUG:
				return "DEBUG";
			case LOG_LEVEL_TRACE:
				return "TRACE";
			default:
				return "DISABLED";
	    }
	}
}
