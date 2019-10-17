/*
 * Copyright (C) 2018
 */

#ifndef LOG_MESSAGE_H
#define LOG_MESSAGE_H

#include "log_utils.h"

namespace lg
{
	struct Log_message
	{
		Log_message(Log_level level_, const std::string& type_, 
				std::string&& text_);
		Log_message(Log_level level_, const std::string& type_, 
				std::string&& text_, std::string&& param1_);

		Log_message(const Log_message&) = delete;
		Log_message& operator=(const Log_message&) = delete;

		static std::string get_time_str(time_t time);
		std::string get_time_str();

		const Log_level level;
		const std::string type;
		const std::string text;
		std::string prefix;
		std::string param1;
		time_t mtime;

		// @ Returns size of the log message content in bytes
		uint32 size() const
		{
			return static_cast<uint32>(prefix.size() + text.size());
		}
	};
}

#endif
