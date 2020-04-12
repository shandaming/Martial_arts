/*
 * Copyright (C) 2019
 */

#ifndef DB_MYSQL_PREPARED_STATEMENT_H
#define DB_MYSQL_PREPARED_STATEMENT_H

#include <mysql/mysql.h>

#include <vector>
#include <string>

// 每个MySQLConnection实例都是唯一的类
//  - 仅在准备好的语句任务时才能访问这些类对象
//  - 被执行。

class prepared_statement_base;

class mysql_prepared_statement
{
	friend class prepared_statement_base;
	friend class mysql_connection;
public:
	mysql_prepared_statement(MYSQL_STMT* stmt);
	~mysql_prepared_statement();

	mysql_prepared_statement(const mysql_prepared_statement&) = delete;
	mysql_prepared_statement& operator=(const mysql_prepared_statement&) = delete;

	void set_null(const uint8_t index);
	void set_bool(const uint8_t index, const bool value);
	void set_uint8(const uint8_t index, const uint8_t value);
	void set_uint16(const uint8_t index, const uint16_t value);
	void set_uint32(const uint8_t index, const uint32_t value);
	void set_uint64(const uint8_t index, const uint64_t value);
	void set_int8(const uint8_t index, const int8_t value);
	void set_int16(const uint8_t index, const int16_t value);
	void set_int32(const uint8_t index, const int32_t value);
	void set_int64(const uint8_t index, const int64_t value);
	void set_float(const uint8_t index, const float value);
	void set_double(const uint8_t index, const double value);
	void set_binary(const uint8_t index, const std::vector<uint8_t>& value, bool is_string);
private:
	MYSQL_STMT* get_stmt() { return stmt_; }
	MYSQL_BIND* get_bind() { return bind_; }

	prepared_statement_base* prepared_stmt_;

	void clear_parameters();
	void check_valid_index(uint8_t index);
	std::string get_query_string(const std::string& sql_pattern) const;

	MYSQL_STMT* stmt_;
	uint32_t param_count_;
	std::vector<bool> params_set_;
	MYSQL_BIND* bind_;
};

#endif
