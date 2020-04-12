/*
 * Copyright (C) 2019
 */

#ifndef DB_PREPARED_STATEMENT_H
#define DB_PREPARED_STATEMENT_H

#include <mysql/mysql.h>

#include "sql_operation.h"

// Union for data buffer（高级绑定 - >队列 - >低级绑定）
union prepared_statement_data_union
{
	bool boolean;
	uint8_t uint8;
	int8_t int8;
	uint16_t uint16;
	int16_t int16;
	uint32_t uint32;
	int32_t int32;
	uint64_t uint64;
	int64_t int64;
	float float_t;
	double double_t;
};

// 这个枚举有助于我们区分上述联合中的数据
enum prepared_statement_value_type
{
	TYPE_BOOL,
	TYPE_UINT8,
	TYPE_UINT16,
	TYPE_UINT32,
	TYPE_UINT64,
	TYPE_INT8,
	TYPE_INT16,
	TYPE_INT32,
	TYPE_INT64,
	TYPE_FLOAT,
	TYPE_DOUBLE,
	TYPE_STRING,
	TYPE_BINARY,
	TYPE_NULL
};

struct prepared_statement_data
{
	prepared_statement_data_union data;
	prepared_statement_value_type type;
	std::vector<uint8_t> binary;
};

class mysql_prepared_statement;

// 代码中使用的上层类
class prepared_statement_base
{
	friend class prepared_statement_task;
	friend class mysql_prepared_statement;
	friend class mysql_connection;
public:
	explicit prepared_statement_base(uint32_t index);
	~prepared_statement_base();

	prepared_statement_base(const prepared_statement_base&) = delete;
	prepared_statement_base& operator=(const prepared_statement_base&) = delete;

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
	void set_string(const uint8_t index, const std::string& value);
	void set_binary(const uint8_t index, const std::vector<uint8_t>& value);
	void set_null(const uint8_t index);
protected:
	void bind_parameters();

	mysql_prepared_statement* stmt_;
	uint32_t index_;
	std::vector<prepared_statement_data> statement_data_; // 参数缓冲区，还没有以任何方式与MySQL绑定
};

template<typename T>
class prepared_statement : public prepared_statement_base
{
public:
	prepared_statement(uint32_t index) : prepared_statement_base(index) {}

	prepared_statement(const prepared_statement&) = delete;
	prepared_statement& operator=(const prepared_statement&) = delete;
};


// 排队操作
class prepared_statement_task : public sql_operation
{
public:
	prepared_statement_task(prepared_statement_base* stmt, bool async = false);
	~prepared_statement_task();

	bool execute() override;
	prepared_query_result_future get_future() { return result_->get_future(); }
private:
	prepared_statement_base* stmt_;
	bool has_result_;
	prepared_query_result_promise* result_;
};

#endif
