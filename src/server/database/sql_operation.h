/*
 * Copyright (C) 2019
 */

#ifndef DATABASE_SQL_OPERATION_H
#define DATABASE_SQL_OPERATION_H

union sql_element_union
{
	prepared_statement* stmt;
	const char* query;
};

enum sql_element_data_type
{
	SQL_ELEMENT_RAW,
	SQL_ELEMENT_PREPARED
};

struct sql_element_data
{
	sql_element_union element;
	sql_element_data_type type;
};

union sql_result_set_union
{
	prepared_result_set* presult;
	result_set* qresult;
};

//class mysql_connection;

class sql_operation
{
public:
	sql_operation() : conn_(NULL) {}
	virtual ~sql_operation() {}

	virtual int call()
	{
		execute();
		return 0;
	}

	virtual bool execute() = 0;
	virtual void set_connection(mysql_connection* conn) { conn_ = conn; }

	mysql_connection* conn;
private:
	sql_operation(const sql_operation&) = delete;
	sql_operation& operator=(const operation&) = delete;
};

#endif
