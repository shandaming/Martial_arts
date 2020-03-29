/*
 * Copyright (C) 2020
 */

#ifndef DB_HOTFIX_DATABASE_H
#define DB_HOTFIX_DATABASE_H

#include "database/mysql_connection.h"

enum hotfix_database_statements : uint32_t
{
	MAX_HOTFIXDATABASE_STATEMENTS
};

class hotfix_database_connection : public mysql_connection
{
public:
	typedef hotfix_database_statements statements;

	hotfix_database_connection(mysql_connection_info& conn_info);
	hotfix_database_connection(producer_consumer_queue<sql_operation*>* q, mysql_connection_info& conn_info);
	~hotfix_database_connection();

	void do_prepare_statements() override;
};

#endif
