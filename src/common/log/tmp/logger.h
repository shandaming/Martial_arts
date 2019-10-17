/*
 * Copyright (C) 2018
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <map>
#include <string>
#include "log_utils.h"
#include "appender.h"
#include "log_message.h"

namespace lg
{
	class Logger
	{
		public:
			Logger(const std::string& name, Log_level level);

			void add_appender(uint8 id, Appender* appender);
			void del_appender(uint8 id);

			const std::string& get_name() const { return name_; }
			Log_level get_log_level() const { return level_; }
			void set_log_level(Log_level level);
			void write(Log_message* message) const;
		private:
			std::string name_;
			Log_level level_;
			std::unordered_map<uint8, Appender*> appenders_;
	};
}

#endif
