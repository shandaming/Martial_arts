/*
 * Copyright (C) 2018
 */

#ifndef COMMON_SYSTEM_INFO_H
#define COMMON_SYSTEM_INFO_H

#include <string>

namespace sys
{
std::string gethostname();

std::string get_sys_datetime();
}

#endif
