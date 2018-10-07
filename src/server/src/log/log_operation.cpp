/*
 * Copyright (C) 2018
 */

#include "log_operation.h"

namespace lg
{
	Log_operation::Log_operation(const Logger* logger, std::unique_ptr<LogMessage>&& msg) : logger_(logger), msg_(std::forward<std::unique_ptr<Log_message>>(msg))
	{
	}

	int Log_operation::call()
	{
	    logger->write(msg_.get());
	    return 0;
	}
}
