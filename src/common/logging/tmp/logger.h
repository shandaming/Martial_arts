/*
 * Copyright (C) 2018
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <unordered_map>
#include <string>

#include "appender.h"

class logger
{
public:
	logger(const std::string& name, log_level level);

	void add_appender(uint8 id, appender* appender);
	void del_appender(uint8 id);

	const std::string& get_name() const { return name_; }
	log_level get_log_level() const { return level_; }
	void set_log_level(log_level level);
	void write(log_message* message) const;
private:
	std::string name_;
	log_level level_;
	std::unordered_map<uint8, appender*> appenders_;
};

#endif
