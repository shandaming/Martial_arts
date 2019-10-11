/*
 * Copyright (C) 2018
 */

#ifndef LOG_OPERATION_H
#define LOG_OPERATION_H

#include <memory>
#include "logger.h"

namespace lg
{
	class Log_operation
	{
		public:
			Log_operation(const Logger* logger, std::unique_ptr<LogMessage>&& msg);

			~Log_operation() {}

			int call();
		private:
			const Logger* logger_;
			std::unique_ptr<Log_message> msg_;
	};
}

#endif
