/*
 * Copyright (C) 2018
 */

#ifndef LOG_OPERATION_H
#define LOG_OPERATION_H

#include <memory>

#include "logger.h"

class log_operation
{
public:
	log_operation(const logger* logger, std::unique_ptr<log_message>&& msg);

	~log_operation() {}

	int call();
private:
	const logger* logger_;
	std::unique_ptr<log_message> msg_;
};

#endif
