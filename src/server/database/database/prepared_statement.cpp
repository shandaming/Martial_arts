/*
 * Copyright (C) 2019
 */

#include <cstring>

#include "prepared_statement.h"
#include "mysql_prepared_statement.h"
#include "mysql_connection.h"
#include "errors.h"

prepared_statement_base::prepared_statement_base(uint32_t index, uint8_t capacity) : stmt_(0), index_(index), statement_data_(capacity) {}

prepared_statement_base::~prepared_statement_base() {}

// 绑定到缓冲区
void prepared_statement_base::set_bool(const uint8_t index, const bool value)
{
	if(index >= statement_data_.size())
		statement_data_.resize(index + 1);

	statement_data_[index].data.boolean = value;
	statement_data_[index].type = TYPE_BOOL;
}

void prepared_statement_base::set_uint8(const uint8_t index, const uint8_t value)
{
	if(index >= statement_data_.size())
		statement_data_.resize(index + 1);

	statement_data_[index].data.uint8 = value;
	statement_data_[index].type = TYPE_UINT8;
}

void prepared_statement_base::set_uint16(const uint8_t index, const uint16_t value)
{
	if(index >= statement_data_.size())
		statement_data_.resize(index + 1);

	statement_data_[index].data.uint16 = value;
	statement_data_[index].type = TYPE_UINT16;
}

void prepared_statement_base::set_uint32(const uint8_t index, const uint32_t value)
{
	if(index >= statement_data_.size())
		statement_data_.resize(index + 1);

	statement_data_[index].data.uint32 = value;
	statement_data_[index].type = TYPE_UINT32;
}

void prepared_statement_base::set_uint64(const uint8_t index, const uint64_t value)
{
	if(index >= statement_data_.size())
		statement_data_.resize(index + 1);

	statement_data_[index].data.uint64 = value;
	statement_data_[index].type = TYPE_UINT64;
}

void prepared_statement_base::set_int8(const uint8_t index, const int8_t value)
{
	if(index >= statement_data_.size())
		statement_data_.resize(index + 1);

	statement_data_[index].data.int8 = value;
	statement_data_[index].type = TYPE_INT8;
}

void prepared_statement_base::set_int16(const uint8_t index, const int16_t value)
{
	if(index >= statement_data_.size())
		statement_data_.resize(index + 1);

	statement_data_[index].data.int16 = value;
	statement_data_[index].type = TYPE_INT16;
}

void prepared_statement_base::set_int32(const uint8_t index, const int32_t value)
{
	if(index >= statement_data_.size())
		statement_data_.resize(index + 1);

	statement_data_[index].data.int32 = value;
	statement_data_[index].type = TYPE_INT32;
}

void prepared_statement_base::set_int64(const uint8_t index, const int64_t value)
{
	if(index >= statement_data_.size())
		statement_data_.resize(index + 1);

	statement_data_[index].data.int64 = value;
	statement_data_[index].type = TYPE_INT64;
}

void prepared_statement_base::set_float(const uint8_t index, const float value)
{
	if(index >= statement_data_.size())
		statement_data_.resize(index + 1);

	statement_data_[index].data.float_t = value;
	statement_data_[index].type = TYPE_FLOAT;
}

void prepared_statement_base::set_double(const uint8_t index, const double value)
{
	if(index >= statement_data_.size())
		statement_data_.resize(index + 1);

	statement_data_[index].data.double_t = value;
	statement_data_[index].type = TYPE_DOUBLE;
}

void prepared_statement_base::set_string(const uint8_t index, const std::string& value)
{
	if(index >= statement_data_.size())
		statement_data_.resize(index + 1);

	statement_data_[index].binary.resize(value.size() + 1);
	memcpy(statement_data_[index].binary.data(), value.c_str(), value.size() + 1);
	statement_data_[index].type = TYPE_STRING;
}

void prepared_statement_base::set_binary(const uint8_t index, const std::vector<uint8_t>& value)
{
	if(index >= statement_data_.size())
		statement_data_.resize(index + 1);

	statement_data_[index].binary = value;
	statement_data_[index].type = TYPE_BINARY;
}

void prepared_statement_base::set_null(const uint8_t index)
{
	if(index >= statement_data_.size())
		statement_data_.resize(index + 1);

	statement_data_[index].type = TYPE_NULL;
}

void prepared_statement_base::bind_parameters()
{
	ASSERT(stmt_);

	uint8_t i = 0;
	for(; i < statement_data_.size(); ++i)
	{
		switch(statement_data_[i].type)
		{
			case TYPE_BOOL:
				stmt_->set_bool(i, statement_data_[i].data.boolean);
				break;
			case TYPE_UINT8:
				stmt_->set_uint8(i, statement_data_[i].data.uint8);
				break;
			case TYPE_UINT16:
				stmt_->set_uint16(i, statement_data_[i].data.uint16);
				break;
			case TYPE_UINT32:
				stmt_->set_uint32(i, statement_data_[i].data.uint32);
				break;
			case TYPE_UINT64:
				stmt_->set_uint64(i, statement_data_[i].data.uint64);
				break;
			case TYPE_INT8:
				stmt_->set_int8(i, statement_data_[i].data.int8);
				break;
			case TYPE_INT16:
				stmt_->set_int16(i, statement_data_[i].data.int16);
				break;
			case TYPE_INT32:
				stmt_->set_int32(i, statement_data_[i].data.int32);
				break;
			case TYPE_INT64:
				stmt_->set_int64(i, statement_data_[i].data.int64);
				break;
			case TYPE_FLOAT:
				stmt_->set_float(i, statement_data_[i].data.float_t);
				break;
			case TYPE_DOUBLE:
				stmt_->set_double(i, statement_data_[i].data.double_t);
				break;
			case TYPE_STRING:
				stmt_->set_binary(i, statement_data_[i].binary, true);
				break;
			case TYPE_BINARY:
				stmt_->set_binary(i, statement_data_[i].binary, false);
				break;
			case TYPE_NULL:
				stmt_->set_null(i);
				break;
			default:
				break;
		}
	}
}


/* ---------------------------- mysql_prepared_statement ----------------------------------- */


/* -------------------------- prepared_statement_task ------------------------------- */

prepared_statement_task::prepared_statement_task(prepared_statement_base* stmt, bool async) : stmt_(stmt), result_(nullptr)
{
	has_result_ = async; // 如果它是异步的，那么就有结果
	if(async)
	{
		result_ = new prepared_query_result_promise();
		ASSERT(result_);
	}
}

prepared_statement_task::~prepared_statement_task()
{
	delete stmt_;
	if(has_result_ && result_ != nullptr)
		delete result_;
}

bool prepared_statement_task::execute()
{
	if(has_result_)
	{
		prepared_result_set* result = conn->query(stmt_);
		if(!result || !result->get_row_count())
		{
			delete result;
			result_->set_value(prepared_query_result(NULL));
			return false;
		}
		result_->set_value(prepared_query_result(result));
		return true;
	}
	return conn->execute(stmt_);
}
