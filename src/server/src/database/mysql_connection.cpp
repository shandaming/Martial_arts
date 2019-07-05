/*
 * Copyright (C) 2019
 */

#include <mysql.h>
#include <mysqld_error.h>

#include "mysql_connection.h"
#include "string_utils.h"

mysql_connection_info::mysql_connection_info(const std::string& info_string){
	std::vector<std::string> tokens = split(info_string, ';');
	if(tokens.size() != 5)
	{
		return;
	}
	uint8_t i = 0;
	host.assign(tokens[i++]);
	port_or_socket.assign(tokens[i++]);
	user.assign(tokens[i++]);
	password.assign(tokens[i++]);
	database.assign(tokens[i++]);
}

mysql_connection::mysql_connection(mysql_connection_info& info) : reconnection_(false), prepare_error_(false), queue_(nullptr), mysql_(nullptr), connection_info_(info), connection_flags_(CONNECTION_SYNCH) {}

mysql_connection::mysql_connection()
{}

mysql_connection::~mysql_connection() { close(); }

uint32_t mysql_connection::open()
{
	MYSQL* mysql_init;
	mysql_init = mysql_init(nullptr);
	if(!mysql_init)
	{
		LOG_ERR << "sql.sql Could not initialize Mysql connection to database " << connection_info_.database;
		return CR_UNKNOWN_ERROR;
	}

	mysql_options(mysql_init, MYSQL_SET_CHAREST_NAME, "utf8");
	int port;
	const char* unix_socket;
	if(connection_info_.host == ".")
	{
		unsigned int opt = MYSQL_PROTOCOL_SOCKET;
		mysql_options(mysql_init, MYSQL_OPT_PROTOCOL, (const char*)&opt);
		connection_info_.host = "localhost";
		port = 0;
		unix_socket = connection_info_.port_or_socket.c_str();
	}
	else
	{
		port = atoi(connection_info_.port_or_socket.c_str());
		unix_socket = nullptr;
	}

	mysql_ = mysql_real_connect(mysql_init, connection_info_.host.c_str(), connection_info_.user.c_str(), connection_info_.password.c_str(), connection_info_.database.c_str(), port, unix_socket, 0);
	if(mysql_)
	{
		if(!reconnection)
		{
			LOG_INFO << "sql.sql MySQL client library: " << mysql_get_client_info();
			LOG_INFO << "sql.sql MySQL server ver: " << mysql_get_server_info(mysql_);
		}

		LOG_INFO << "sql.sql Connected to MySQL database at " << connection_info_.host;
		mysql_autocommit(mysql_, 1);

		//将连接属性设置为UTF8以正确处理不同的语言环境
		// server configs  -  core以UTF8发送数据，因此MySQL也必须期待UTF8
		mysql_set_character_set(mysql_, "utf8");
		return 0;
	}
	else
	{
		LOG_ERR << "sql.sql Could not connect to MySQL database at " << connection_info_.host << " : " << mysql_error(mysql_init);
		mysql_close(mysql_init);
		return mysql_errno(mysql_init);
	}
}

void mysql_connection::close()
{
	// 在清除之前停止工作线程
	worker_.reset();
	stmts_.clear();

	if(mysql_)
	{
		mysql_close(mysql_);
		mysql_ = nullptr;
	}
}

bool mysql_connection::prepare_statements()
{
	do_prepare_statements();
	return !prepare_error_;
}

bool mysql_connection::execute(const char* sql)
{
	if(!mysql_)
	{
		return false;
	}

	uint32_t ms = get_ms_time();
	if(mysql_query(mysql_, sql))
	{
		uint32_t errno = mysql_errno(mysql_);

		LOG_INFO << "sql.sql SQL:" << sql;
		LOG_ERROR << "sql.sql [" << errno << "] " << mysql_error(mysql_);

		if(handle_mysql_errno(errno)) // 如果返回true，则成功处理错误（即重新连接）
		{
			return execute(sql); //再试一次
		}
		return false;
	}
	else
	{
		LOG_DEBUG << "sql.sql [" << get_ms_time_diff << " ms] SQL:" << sql;
	}
	return true;
}
