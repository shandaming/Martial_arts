/*
 * Copyright (C) 2018
 */

#ifndef APPENDER_H
#define APPENDER_H

#include "log_utils.h"

namespace lg
{
	class Appender
	{
		public:
			Appender(uint8 id, const std::string& name, Log_level level = LOG_LEVEL_DISABLED, Appender_flags flags = APPENDER_FLAGS_NONE);
			virtual ~Appender() {}

			uint8 get_id() const { return id_; }
			std::string const& get_name() const { return name_; }
			Log_level get_log_level() const { return level_; }
			Appender_flags get_flags() const { return flags_; }
			virtual Appender_type get_type() const = 0;

			void set_log_level(Log_level) { level_ = level; }
			void write(Log_message* message);
			static const char* get_log_level_str(Log_level level);
			virtual void set_realm_id(uint32) { }
		private:
			virtual void write_stream(const Log_message*) = 0;

			uint8 id_;
			std::string name_;
			Log_level level_;
			Appender_flags flags_;
	};

	class Invalid_appender_args_exception : public std::length_error
	{
	public:
	    explicit Invalid_appender_args_exception(const std::string& message) : std::length_error(message) { }
	};
}

#endif
