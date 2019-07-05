/*
 * Copyright (C) 2019
 */

#ifndef DATABASE_QUERY_RESULT_H
#define DATABASE_QUERY_RESULT_H

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

	field* fetch() const { return current_row_; }
	const field& operator[](size_t index) const;
private:
	uint64_t row_count_;
	field* current_row_;
	uint32_t field_count_;

	void clean_up();

	MYSQL_RES* result_;
	MYSQL_FIELD* fields_;
};

class prepare_result_set
{
public:
	prepare_result_set(MYSQL_STMT* stmt, MYSQL_RES* result, uint64_t row_count, uint32_t field_count);
	~prepare_result_set();

	prepare_result_set(const prepare_result&) = delete;
	prepare_result_set& operator=(const prepare_result&) = delete;

	bool next_row();
	uint64_t get_row_count() const { return row_count_; }
	uint32_t get_field_count() const { return field_count_; }

	field* fetch() const;
	const field* operator[](size_t index) const;
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

#endif
