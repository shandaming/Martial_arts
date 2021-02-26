/*
 * Copyright (C) 2018
 */

#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <ctime>

#include "system_info.h"

namespace sys
{
std::string gethostname()
{
	char hostname[128] = {0};
	if(::gethostname(hostname, sizeof(hostname)) < 0)
	{
		return strerror(errno);
	}
	return hostname;
}

std::string get_sys_datetime()
{
	std::time_t t = std::time(NULL);
    	char mbstr[100];
    	std::strftime(mbstr, sizeof(mbstr), "%F %T", std::localtime(&t));
	return mbstr;
}
}
