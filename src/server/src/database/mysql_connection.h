/*
 * Copyright (C) 2019
 */

#ifndef DATABASE_MYSQL_CONNECTION_H
#define DATABASE_MYSQL_CONNECTION_H

enum connection_flags
{
	CONNECTION_ASYNC = 0x01,
	CONNECTION_SYNCH = 0x02,
	CONNECTION_BOTH = CONNECTION_ASYNC | CONNECTION_SYNCH
};

struct mysql_connection_info
{
	explicit mysql_connection_info(const std::string& info_string);

	std::string user;
	std::string password;
	std::string database;
	std::string host;
	std::string port_or_socket;
};

#endif
