/*
 * Copyright (C) 2019
 */

#ifndef CHARACTER_DATABASE_H
#define CHARACTER_DATABASE_H

#include "database/mysql_connection.h"

enum character_database_statements : uint32_t
{

	MAX_CHARACTERDATABASE_STATEMENTS
};

class character_database_connection : public mysql_connection
{
public:
	typedef character_database_statements statements;

	character_database_connection(mysql_connection_info& conn_info);
	character_database_connection(producer_consumer_queue<sql_operation*>* q, mysql_connection_info& conn_info);
	~character_database_connection();

	void do_prepare_statements() override;
};

#endif
