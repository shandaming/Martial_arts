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

