/*
 * Copyright (C) 2018
 */

#ifndef APPENDER_H
#define APPENDER_H

#include <string>
#include <stdexcept>

#include "log_message.h"

class appender
{
public:
	appender(uint8_t id, const std::string& name, log_level level = LOG_LEVEL_DISABLED, 
			appender_flags flags = APPENDER_FLAGS_NONE);
	virtual ~appender() {}

	uint8_t get_id() const { return id_; }
	std::string const& get_name() const { return name_; }
	log_level get_log_level() const { return level_; }
	appender_flags get_flags() const { return flags_; }
	virtual appender_type get_type() const = 0;

	void set_log_level(log_level level) { level_ = level; }
	void write(log_message* message);
	static const char* get_log_level_str(log_level level);
	virtual void set_realm_id(uint32_t) { }
private:
	virtual void write_stream(const log_message*) = 0;

	uint8_t id_;
	std::string name_;
	log_level level_;
	appender_flags flags_;
};

struct invalid_appender_args_exception : std::length_error
{
	explicit invalid_appender_args_exception(const std::string& message) : std::length_error(message) {}
};

#endif
