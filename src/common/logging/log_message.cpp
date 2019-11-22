/*
 * Copyright (C) 2018 by Shan Daming
 */

#include <cstdio>

#include "log_message.h"

log_message::log_message(log_level level_, const std::string& type_, std::string&& text_) :
	level(level_), type(type_), text(std::forward<std::string>(text_)), mtime(time(nullptr)){}

log_message::log_message(log_level level_, const std::string& type_, std::string&& text_, 
		std::string&& param1_) : 
	level(level_), type(type_), text(std::forward<std::string>(text_)), 
	param1(std::forward<std::string>(param1)), mtime(time(nullptr)){}

std::string log_message::get_time_str(time_t time)
{
	tm aTm;
	localtime_r(&time, &aTm);
	char buf[20];
	snprintf(buf, 20, "%04d-%02d-%02d_%02d:%02d:%02d", aTm.tm_year + 1900, aTm.tm_mon + 1, 
			aTm.tm_mday, aTm.tm_hour, aTm.tm_min, aTm.tm_sec);
	return std::string(buf);
}

std::string log_message::get_time_str()
{
	return get_time_str(mtime);
}
