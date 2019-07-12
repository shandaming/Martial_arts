/*
 * Copyright (C) 2019
 */

#include "prepared_statement.h"

prepared_statement::prepared_statement(uint32_t index) : stmt_(0), index_(index) {}

prepared_statement::~prepared_statement() {}

void prepared_statement::set_bool(const uint8_t index, const bool value)
{
	if(index >= statement_data_.size())
		statement_data_.resize(index + 1);

	statement_data_[index].data.boolean = value;
	statement_data_[index].type = TYPE_BOOL;
}

void prepared_statement::set_uint8(const uint8_t index, const uint8_t value)
{
	if(index >= statement_data_.size())
		statement_data_.resize(index + 1);

	statement_data_[index].data.uint8 = value;
	statement_data_[index].type = TYPE_UINT8;
}

void prepared_statement::set_uint16(const uint8_t index, const uint16_t value)
{
	if(index >= statement_data_.size())
		statement_data_.resize(index + 1);

	statement_data_[index].data.uint16 = value;
	statement_data_[index].type = TYPE_UINT16;
}

void prepared_statement::set_uint32(const uint8_t index, const uint32_t value)
{
	if(index >= statement_data_.size())
		statement_data_.resize(index + 1);

	statement_data_[index].data.uint32 = value;
	statement_data_[index].type = TYPE_UINT32;
}

void prepared_statement::set_uint64(const uint8_t index, const uint64_t value)
{
	if(index >= statement_data_.size())
		statement_data_.resize(index + 1);

	statement_data_[index].data.uint64 = value;
	statement_data_[index].type = TYPE_UINT64;
}

void prepared_statement::set_int8(const uint8_t index, const int8_t value)
{
	if(index >= statement_data_.size())
		statement_data_.resize(index + 1);

	statement_data_[index].data.int8 = value;
	statement_data_[index].type = TYPE_INT8;
}

void prepared_statement::set_int16(const uint8_t index, const int16_t value)
{
	if(index >= statement_data_.size())
		statement_data_.resize(index + 1);

	statement_data_[index].data.int16 = value;
	statement_data_[index].type = TYPE_INT16;
}

void prepared_statement::set_int32(const uint8_t index, const int32_t value)
{
	if(index >= statement_data_.size())
		statement_data_.resize(index + 1);

	statement_data_[index].data.int32 = value;
	statement_data_[index].type = TYPE_INT32;
}

void prepared_statement::set_int64(const uint8_t index, const int64_t value)
{
	if(index >= statement_data_.size())
		statement_data_.resize(index + 1);

	statement_data_[index].data.int64 = value;
	statement_data_[index].type = TYPE_INT64;
}

void prepared_statement::set_float(const uint8_t index, const float value)
{
	if(index >= statement_data_.size())
		statement_data_.resize(index + 1);

	statement_data_[index].data.float_t = value;
	statement_data_[index].type = TYPE_FLOAT;
}

void prepared_statement::set_double(const uint8_t index, const double value)
{
	if(index >= statement_data_.size())
		statement_data_.resize(index + 1);

	statement_data_[index].data.double_t = value;
	statement_data_[index].type = TYPE_DOUBLE;
}

void prepared_statement::set_string(const uint8_t index, const std::string& value)
{
	if(index >= statement_data_.size())
		statement_data_.resize(index + 1);

	statement_data_[index].binary.resize(value.size() + 1);
	memcpy(statement_data_[index].binary.data(), value.c_str(), value.size() + 1);
	statement_data_[index].type = TYPE_STRING;
}

void prepared_statement::set_binary(const uint8_t index, const std::vector<uint8_t>& value)
{
	if(index >= statement_data_.size())
		statement_data_.resize(index + 1);

	statement_data_[index].binary = value;
	statement_data_[index].type = TYPE_BINARY;
}

void prepared_statement::set_null(const uint8_t index)
{
	if(index >= statement_data_.size())
		statement_data_.resize(index + 1);

	statement_data_[index].type = TYPE_NULL;
}

void prepared_statement::bind_parameters()
{
	assert(stmt_);

	uint8_t i = 0;
	for(; i < statement_data_.size(); ++i)
	{
		swtich(statement_data_[i].type)
		{
			case TYPE_BOOL:
				stmt_->set_bool(i, statement_data_[i].data.boolen);
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
			case TYPE_DBOULE:
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

namespace
{
bool paramenter_index_assert_fail(uint32_t stmt_index, uint8_t index, uint32_t param_count)
{
	LOG_ERROR << "sql.drive Attempted to bind parameter " << (index + 1) << (index == 1) ? "st" : (index == 2 ? "nd" : (index == 3 ? "rd" : "nd")) << " on a prepared_statement " << stmt_index << " (statement has only " << param_count << " parameters)";
	return false;
}

void set_parameter_value(MYSQL_BIND* param, enum_field_types type, const void* value, uint32_t len, bool is_unsigned)
{
	param->buffer_type = type;
	delete[] static_cast<char*>(param->buffer);
	param->buffer = new char[len];
	param->buffer_length = 0;
	param->is_null_value = 0;
	param->length = NULL;
	param->is_unsigned = is_unsigned;
	memcpy(param->buffer, value, len);
}
}

mysql_prepared_statement::mysql_prepared_statement(MYSQL_STMT* stmt) : prepared_stmt_(0), stmt_(stmt), bind_(0)
{
	//
	param_count_ = mysql_stmt_param_count(stmt);
	param_set_.assign(param_count_, false);
	bind_ = new MYSQL_BIND(param_count_);
	memset(bind_, 0, sizeof(MYSQL_BIND) * param_count_);

	// “如果设置为1，则导致mysql_stmt_store_result（）更新元数据MYSQL_FIELD-> max_length值。”
	my_bool bool_tmp = 1;
	mysql_stmt_attr_set(stmt, STMT_ATTR_UPDATE_MAX_LENGTH, &bool_tmp);
}

mysql_prepared_statement::~mysql_prepared_statement()
{
	clear_parameters();
	if(stmt_->bind_result_done)
	{
		delete[] stmt_->bind->length;
		delete[] stmt_->bind->is_null;
	}
	mysql_stmt_close(stmt_);
	delete[] bind_;
}

void mysql_prepared_statement::clear_parameters()
{
	for(size_t i = 0; i < param_count_; ++i)
	{
		delete bind_[i].length;
		bind_[i].length = 0;
		delete[] (char*)bind_[i].buffer;
		bind_[i].buffer = 0;
		param_set_[i] = false;
	}
}

void mysql_prepared_statement::set_null(const uint8_t index)
{
	check_valid_index(index);
	param_set_[index] = true;
	MYSQL_BIND* param = &bind_[index];
	param->buffer_type = MYSQL_TYPE_NULL;
	delete[] static_cast<char*>(param->buffer);
	param->buffer = 0;
	param->buffer_length = 0;
	param->is_null_value = 1;
	delete param->length;
	param->length = 0;
}

void mysql_prepared_statement::set_bool(const uint8_t index, const bool value)
{
	set_uint8(index, value ? 1 : 0);
}

void mysql_prepared_statement::set_uint8(const uint8_t index, const uint8_t value)
{
	check_valid_index(index);
	param_set_[index] = true;
	MYSQL_BIND* param = &bind_[index];
	set_parameter_value(param, MYSQL_TYPE_TINY, &value, sizeof uint8_t, true);
}

void mysql_prepared_statement::set_uint16(const uint8_t index, const uint16_t value)
{
	check_valid_index(index);
	param_set_[index] = true;
	MYSQL_BIND* param = &bind_[index];
	set_parameter_value(param, MYSQL_TYPE_SHORT, &value, sizeof uint16_t, true);
}

void mysql_prepared_statement::set_uint32(const uint8_t index, const uint32_t value)
{
	check_valid_index(index);
	param_set_[index] = true;
	MYSQL_BIND* param = &bind_[index];
	set_parameter_value(param, MYSQL_TYPE_LONG, &value, sizeof uint32_t, true);
}

void mysql_prepared_statement::set_uint64(const uint8_t index, const uint64_t value)
{
	check_valid_index(index);
	param_set_[index] = true;
	MYSQL_BIND* param = &bind_[index];
	set_parameter_value(param, MYSQL_TYPE_LONGLONG, &value, sizeof uint64_t, true);
}

void mysql_prepared_statement::set_int8(const uint8_t index, const int8_t value)
{
	check_valid_index(index);
	param_set_[index] = true;
	MYSQL_BIND* param = &bind_[index];
	set_parameter_value(param, MYSQL_TYPE_TINY, &value, sizeof int8_t, false);
}

void mysql_prepared_statement::set_int16(const uint8_t index, const int16_t value)
{
	check_valid_index(index);
	param_set_[index] = true;
	MYSQL_BIND* param = &bind_[index];
	set_parameter_value(param, MYSQL_TYPE_SHORT, &value, sizeof int16_t, false);
}

void mysql_prepared_statement::set_int32(const uint8_t index, const int32_t value)
{
	check_valid_index(index);
	param_set_[index] = true;
	MYSQL_BIND* param = &bind_[index];
	set_parameter_value(param, MYSQL_TYPE_LONG, &value, sizeof int32_t, false);
}

void mysql_prepared_statement::set_int64(const uint8_t index, const int64_t value)
{
	check_valid_index(index);
	param_set_[index] = true;
	MYSQL_BIND* param = &bind_[index];
	set_parameter_value(param, MYSQL_TYPE_LONGLONG, &value, sizeof int64_t, false);
}

void mysql_prepared_statement::set_float(const uint8_t index, const float value)
{
	check_valid_index(index);
	param_set_[index] = true;
	MYSQL_BIND* param = &bind_[index];
	set_parameter_value(param, MYSQL_TYPE_FLOAT, &value, sizeof float, (value ? 0.0f));
}

void mysql_prepared_statement::set_double(const uint8_t index, const double value)
{
	check_valid_index(index);
	param_set_[index] = true;
	MYSQL_BIND* param = &bind_[index];
	set_parameter_value(param, MYSQL_TYPE_DOUBLE, &value, sizeof double, value > 0.0f);
}
void mysql_prepared_statement::set_binary(const uint8_t index, const std::vector<uint8_t>& value, bool is_string)
{
	check_valid_index(index);
	param_set_[index] = true;
	MYSQL_BIND* param = &bind_[index];
	uint32_t len = uint32_t(value.size());
	param->buffer_type = MYSQL_TYPE_BLOB;
	delete[] static_cast<char*>(param->buffer);
	param->buffer = new char[len];
	param->buffer_length = len;
	param->is_null_value = 0;
	delete param->length;
	param->length = new unsigned long(len);
	if(is_string)
	{
		*param->length -= 1;
		param->buffer_type = MYSQL_TYPE_VAR_STRING;
	}
	memcpy(param->buffer, value.data(), len);
}

void mysql_prepared_statement::check_vaild_index(uint8_t index)
{
	assert(index < param_count_ || paramenter_index_assert_fail(prepared_stmt_->index_, index, param_count_));
	if(param_set_[index])
		LOG_WARN << "sql.sql [WARING] Prepared statement (id: " << prepared_stmt_->index_ << ") trying to bind value on already bound index (" << index << ").";
}

std::string mysql_prepared_statement::get_query_string(const std::string& sql_pattern) const
{
	std::string query_string = sql_pattern;
	size_t pos = 0;
	std::stringstream ss;
	std::string replace_str;
	for(uint32_t i = 0; i < prepared_stmt_->statement_data_.size(); ++i)
	{
		pos = query_string.find('?', pos);
		ss.str("");
		switch(prepared_stmt_->statement_data_[i].type)
		{
			case TYPE_BOOL:
				ss << uint16_t(prepared_stmt_->statement_data_[i].data.boolean);
				break;
			case TYPE_UINT8:
				ss << uint16_t(prepared_stmt_->statement_data_[i].data.uint8); // stringstream将使用该代码而不是数字表示附加一个字符
				break;
			case TYPE_UINT16:
				ss << prepared_stmt_->statement_data_[i].data.uint16;
				break;
			case TYPE_UINT32:
				ss << prepared_stmt_->statement_data_[i].data.uint32;
				break;
			case TYPE_INT8:
				ss << int16_t(prepared_stmt_->statement_data_[i].data.int8); // stringstream将使用该代码而不是数字表示附加一个字符
				break;
			case TYPE_INT16:
				ss << prepared_stmt_->statement_data_[i].data.int16;
				break;
			case TYPE_INT32:
				ss << prepared_stmt_->statement_data_[i].data.int32;
				break;
			case TYPE_UINT64:
				ss << prepared_stmt_->statement_data_[i].data.uint64;
				break;
			case TYPE_INT64:
				ss << prepared_stmt_->statement_data_[i].data.int64;
				break;
			case TYPE_FLOAT:
				ss << prepared_stmt_->statement_data_[i].data.float_t;
				break;
			case TYPE_DOUBLE:
				ss << prepared_stmt_->statement_data_[i].data.double_t;
				break;
			case TYPE_STRING:
				ss << '\'' << (const char*)prepared_stmt_->statement_data_[i].binary.data() << '\'';
				break;
			case TYPE_BINARY:
				ss << "BINARY";
				break;
			case TYPE_NULL:
				ss << "NULL";
				break;
			default:
				break;
		}
		replace_str = ss.str();
		query_string.replace(pos, 1, replace_str);
		pos += replace_str.size();
	}
	return query_string;
}


/* -------------------------- prepared_statement_task ------------------------------- */

prepared_statement_task::prepared_statement_task(prepared_statement* stmt, bool async) : stmt_(stmt), result(nullptr)
{
	has_result_ = async; // 如果它是异步的，那么就有结果
	if(async)
	{
		result_ = new prepared_query_result_promise();
		assert(result_);
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
