/*
 * Copyright (C) 2019
 */

#ifndef DATABASE_QUERY_RESULT_H
#define DATABASE_QUERY_RESULT_H

#include <future>
#include <mysql/mysql.h>

#include "field.h"

// 结果集
class result_set
{
public:
	result_set(MYSQL_RES* result, MYSQL_FIELD* fields, uint64_t row_count, uint32_t field_count);
	~result_set();

	result_set(const result_set&) = delete;
	result_set& operator=(const result_set&) = delete;

	bool next_row();
	uint64_t get_row_count() const { return row_count_; }
	uint32_t get_field_count() const { return field_count_; }

	// 取当前行
	field* fetch() const { return current_row_; }
	// 按索引取参数
	const field& operator[](size_t index) const;
private:
	uint64_t row_count_;
	field* current_row_;
	uint32_t field_count_;

	void clean_up();

	MYSQL_RES* result_;
	MYSQL_FIELD* fields_;
};

typedef std::shared_ptr<result_set> query_result;
typedef std::future<query_result> query_result_future;
typedef std::promise<query_result> query_result_promise;



class prepared_result_set
{
public:
	prepared_result_set(MYSQL_STMT* stmt, MYSQL_RES* result, uint64_t row_count, uint32_t field_count);
	~prepared_result_set();

	prepared_result_set(const prepared_result_set&) = delete;
	prepared_result_set& operator=(const prepared_result_set&) = delete;

	bool next_row();
	uint64_t get_row_count() const { return row_count_; }
	uint32_t get_field_count() const { return field_count_; }

	field* fetch() const;
	const field operator[](size_t index) const;
private:
	void clean_up();
	bool goto_next_row();

	std::vector<field> rows_;
	uint64_t row_count_;
	uint64_t row_position_;
	uint32_t field_count_;

	MYSQL_BIND* bind_;
	MYSQL_STMT* stmt_;
	MYSQL_RES* metadata_result_; // 字段元数据，由mysql_stmt_result_metadata返回
};

typedef std::shared_ptr<prepared_result_set> prepared_query_result;
typedef std::future<prepared_query_result> prepared_query_result_future;
typedef std::promise<prepared_query_result> prepared_query_result_promise;

#endif
