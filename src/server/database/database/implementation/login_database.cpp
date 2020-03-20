/*
 * Copyright (C) 2020
 */

#include "login_database.h"
#include "database/mysql_prepared_statement.h"

void login_database_connection::do_prepare_statements()
{
	if(!reconnection_)
		stmts_.resize(MAX_LOGINDATABASE_STATEMENTS);
}

login_database_connection::login_database_connection(mysql_connection_info& conn_info) : mysql_connection(conn_info) {}

login_database_connection::login_database_connection(producer_consumer_queue<sql_operation*>* q, mysql_connection_info& conn_info) : mysql_connection(q, conn_info) {}

login_database_connection::~login_database_connection() {}