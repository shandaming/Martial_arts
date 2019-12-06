/*
 * Copyright (C) 2019
 */

#include "revision_data.h"

namespace revision
{
const char* get_mysql_executable()
{
	return MYSQL_EXECUTABLE;
}

const char* get_source_directory()
{
	return SOURCE_DIRECTORY;
}

const char* get_full_database()
{
	return FULL_DATABASE;
}

const char* get_hotfixes_database()
{
	return HOTFIXES_DATABASE;
}

const char* get_full_version()
{
	return "";
}
}
