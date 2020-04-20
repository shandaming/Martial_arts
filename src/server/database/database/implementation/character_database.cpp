/*
 * Copyright (C) 2019
 */

#include "character_database.h"
#include "mysql_prepared_statement.h"

character_database_connection::character_database_connection(mysql_connection_info& conn_info) : mysql_connection(conn_info) {}

character_database_connection::character_database_connection(producer_consumer_queue<sql_operation*>* q, mysql_connection_info& conn_info) :
	mysql_connection(q, conn_info) 
{}

character_database_connection::~character_database_connection()
{}

void character_database_connection::do_prepare_statements()
{
	if(!reconnection_)
		stmts_.resize(MAX_CHARACTERDATABASE_STATEMENTS);


}
