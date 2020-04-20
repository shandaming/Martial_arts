/*
 * Copyright (C) 2020
 */

#include "hotfix_database.h"
#include "mysql_prepared_statement.h"

void hotfix_database_connection::do_prepare_statements()
{
	if(!reconnection_)
		stmts_.resize(MAX_HOTFIXDATABASE_STATEMENTS);
}

hotfix_database_connection::hotfix_database_connection(mysql_connection_info& conn_info) : mysql_connection(conn_info) {}

hotfix_database_connection::hotfix_database_connection(producer_consumer_queue<sql_operation*>* q, mysql_connection_info& conn_info) : mysql_connection(q, conn_info) {}

hotfix_database_connection::~hotfix_database_connection() {}
