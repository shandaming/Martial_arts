/*
 * Copyright (C) 2019
 */

#ifndef MYSQL_THREADING_H
#define MYSQL_THREADING_H

namespace mysql
{
void library_init();
void library_end();
const char* get_library_version();
}

#endif
