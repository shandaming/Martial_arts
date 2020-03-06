/*
 * Coprygith (C) 2020
 */

#include "world_database.h"
#include "database/mysql_prepared_statement.h"

void world_database_connection::do_prepare_statements()
{
	if(!reconnection_)
		stmts_.resize(MAX_WORLDDATABASE_STATEMENTS);
}

world_database_connection::world_database_connection(mysql_connection_info& conn_info) : mysql_connection(conn_info) {}

world_database_connection::world_database_connection(producer_consumer_queue<sql_operation*>* q, mysql_connection_info& conn_info) : mysql_connection(q, conn_info) {}

world_database_connection::~login_database_connection() {}
