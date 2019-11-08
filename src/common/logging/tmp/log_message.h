/*
 * Copyright (C) 2018
 */

#ifndef LOG_MESSAGE_H
#define LOG_MESSAGE_H

#include <string>
#include <ctime>

#include "log_common.h"

struct log_message
{
	log_message(Log_level level_, const std::string& type_, std::string&& text_);
	log_message(Log_level level_, const std::string& type_, std::string&& text_, 
			std::string&& param1_);

	log_message(const log_message&) = delete;
	log_message& operator=(const log_message&) = delete;

	static std::string get_time_str(time_t time);
	std::string get_time_str();

	const log_level level;
	const std::string type;
	const std::string text;
	std::string prefix;
	std::string param1;
	time_t mtime;

	uint32 size() const
	{
		return static_cast<uint32>(prefix.size() + text.size());
	}
};

#endif
