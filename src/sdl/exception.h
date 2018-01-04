/*
 * Copyright (C) 2017 by Shan Daming <shandaming@hotmail.com>
 */

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include "../exceptions.h"

struct Exception : public Error
{
	Exception(const std::string& operation, const bool use_sdl_error);
};

#endif
