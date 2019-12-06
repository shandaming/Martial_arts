/*
 * Copyright (C) 2019
 */

#ifndef REVISION_H
#define REVISION_H

namespace revision
{
const char* get_mysql_executable();

const char* get_source_directory();

const char* get_full_database();

const char* get_hotfixes_database();

const char* get_full_version();
}

#endif
