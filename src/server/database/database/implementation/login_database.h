/*
 * Copyright (C) 2020
 */

#ifndef DB_LOGIN_DATABASE_H
#define DB_LOGIN_DATABASE_H

#include "mysql_connection.h"

enum login_database_statements : uint32_t
{
	LOGIN_SEL_REALMLIST,
	LOGIN_SEL_IP_INFO,
	LOGIN_INS_LOG,

	MAX_LOGINDATABASE_STATEMENTS
};

class login_database_connection : public mysql_connection
{
public:
	typedef login_database_statements statements;

	login_database_connection(mysql_connection_info& conn_info);
	login_database_connection(producer_consumer_queue<sql_operation*>* q, mysql_connection_info& conn_info);
	~login_database_connection();

	void do_prepare_statements() override;
};

#endif
