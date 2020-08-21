/*
 * Copyright (C) 2020
 */

#include "login_database.h"
#include "mysql_prepared_statement.h"

void login_database_connection::do_prepare_statements()
{
	if(!reconnection_)
		stmts_.resize(MAX_LOGINDATABASE_STATEMENTS);

	prepare_statement(LOGIN_SEL_IP_INFO, "SELECT unbandate > UNIX_TIMESTAMP() OR unbandate = bandate AS banned, NULL as country FROM ip_banned WHERE ip = ?", CONNECTION_ASYNC);
	prepare_statement(LOGIN_INS_LOG, "INSERT INTO logs (time, realm, type, level, string) VALUES (?, ?, ?, ?, ?)", CONNECTION_ASYNC);
}

login_database_connection::login_database_connection(mysql_connection_info& conn_info) : mysql_connection(conn_info) {}

login_database_connection::login_database_connection(producer_consumer_queue<sql_operation*>* q, mysql_connection_info& conn_info) : mysql_connection(q, conn_info) {}

login_database_connection::~login_database_connection() {}
