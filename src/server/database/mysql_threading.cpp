/*
 * Copyright (C) 2019
 */

#include <mysql/mysql.h>

#include "mysql_threading.h"

void mysql::library_init()
{
	mysql_library_init(-1, NULL, NULL);
}

void mysql::library_end()
{
	mysql_library_end();
}

const char* get_library_vserion()
{
	return MYSQL_SERVER_VERSION;
}
