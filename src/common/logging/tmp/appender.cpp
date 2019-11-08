/*
 * Copyright (C) 2018
 */

#include <sstream>
#include "appender.h"
#include "string_utils.h"

appender::appender(uint8 id, const std::string& name, log_level level, appender_flags flags) : 
	id_(id), name_(name), level_(level), flags_(flags) { }

void appender::write(log_message* message)
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
				appender::get_log_level_str(message->level));
	}

	if(flags_ & APPENDER_FLAGS_PREFIX_LOGFILTERTYPE)
	{
		ss << '[' << message->type << "] ";
	}

	message->prefix = ss.str();
	write_stream(message);
}

const char* appender::get_log_level_str(log_level level)
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
