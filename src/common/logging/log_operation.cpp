/*
 * Copyright (C) 2018
 */

#include "log_operation.h"

log_operation::log_operation(const logger* logger, std::unique_ptr<log_message>&& msg) : 
	logger_(logger), msg_(std::forward<std::unique_ptr<log_message>>(msg))
{}

int log_operation::call()
{
	logger_->write(msg_.get());
	return 0;
}
