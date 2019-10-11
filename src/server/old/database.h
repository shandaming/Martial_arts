/*
 * Copyright (C) 2018
 */

#ifndef DATABASE_H
#define DATABASE_H

#include <vector>
#include <string>
#include <mysql/mysql.h>

namespace db
{
	// SQL-select
	std::vector<std::vector<std::string>> query_data(MYSQL* mysql, 
			const std::string& sql);
	// SQL-insert
	bool insert_data(MYSQL* mysql, const std::string& sql);
	// SQL-update
	bool update_data(MYSQL* mysql, const std::string& sql);
	// SQL-delete
	bool delete_data(MYSQL* mysql, const std::string& sql);
}

#endif
