/*
 * Copyright (C) 2020
 */

#ifndef WORLD_DATABASE_H
#define WORLD_DATABASE_H

#include "database/mysql_connection.h"

enum world_database_statements : uint32_t
{
	MAX_WORLDDATABASE_STATEMENTS
};

class world_database_connection : public mysql_connection
{
public:
	typedef world_database_statements statements;

	world_database_connection(mysql_connection_info& conn_info);
	world_database_connection(producer_consumer_queue<sql_operation*>* q, mysql_connection_info& conn_info);
	~world_database_connection();

	void do_prepare_statements() override;
};

#endif
