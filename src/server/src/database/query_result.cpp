/*
 * Copyright (C) 2019
 */

#include "query_result.h"

static uint32_t size_for_type(MYSQL_FIELD* field)
{
	switch(field->type)
	{
		case MYSQL_TYPE_NULL:
			return 0;
		case MYSQL_TYPE_TINY:
            return 1;
		case MYSQL_TYPE_YEAR:
		case MYSQL_TYPE_SHORT:
		    return 2;
		case MYSQL_TYPE_INT24:
		case MYSQL_TYPE_LONG:
		case MYSQL_TYPE_FLOAT:
		    return 4;
		case MYSQL_TYPE_DOUBLE:
		case MYSQL_TYPE_LONGLONG:
		case MYSQL_TYPE_BIT:
		    return 8;

		case MYSQL_TYPE_TIMESTAMP:
		case MYSQL_TYPE_DATE:
		case MYSQL_TYPE_TIME:
		case MYSQL_TYPE_DATETIME:
		    return sizeof(MYSQL_TIME);

		case MYSQL_TYPE_TINY_BLOB:
		case MYSQL_TYPE_MEDIUM_BLOB:
		case MYSQL_TYPE_LONG_BLOB:
		case MYSQL_TYPE_BLOB:
		case MYSQL_TYPE_STRING:
		case MYSQL_TYPE_VAR_STRING:
		    return field->max_length + 1;

		case MYSQL_TYPE_DECIMAL:
		case MYSQL_TYPE_NEWDECIMAL:
		    return 64;

		case MYSQL_TYPE_GEOMETRY:
		    /*
		    以下类型不通过电线发送：
		    MYSQL_TYPE_ENUM:
		    MYSQL_TYPE_SET:
		    */
		default:
			LOG_WARIN << "sql.sql SQL::size_for_type(): invalid field type " << uint32_t(field->type);
			return 0;
	}
}

database_field_type mysql_type_to_field_type(enum_field_types type)
{
	switch (type)
	{
		case MYSQL_TYPE_NULL:
			return database_field_type::Null;
		case MYSQL_TYPE_TINY:
		    return database_field_type::Int8;
		case MYSQL_TYPE_YEAR:
		case MYSQL_TYPE_SHORT:
		    return database_field_type::Int16;
		case MYSQL_TYPE_INT24:
		case MYSQL_TYPE_LONG:
		    return database_field_type::Int32;
		case MYSQL_TYPE_LONGLONG:
		case MYSQL_TYPE_BIT:
		    return database_field_type::Int64;
		case MYSQL_TYPE_FLOAT:
		    return database_field_type::Float;
		case MYSQL_TYPE_DOUBLE:
		    return database_field_type::Double;
		case MYSQL_TYPE_DECIMAL:
		case MYSQL_TYPE_NEWDECIMAL:
		    return database_field_type::Decimal;
		case MYSQL_TYPE_TIMESTAMP:
		case MYSQL_TYPE_DATE:
		case MYSQL_TYPE_TIME:
		case MYSQL_TYPE_DATETIME:
		    return database_field_type::Date;
		case MYSQL_TYPE_TINY_BLOB:
		case MYSQL_TYPE_MEDIUM_BLOB:
		case MYSQL_TYPE_LONG_BLOB:
		case MYSQL_TYPE_BLOB:
		case MYSQL_TYPE_STRING:
		case MYSQL_TYPE_VAR_STRING:
		    return database_field_type::Binary;
		default:
			LOG_WARIN << "sql.sql mysql_type_to_field_type(): invalid field type " << uint32_t(type);
			break;
	}
	return database_field_type::Null;
}


result_set::result_set(MYSQL_RES* result, MYSQL_FIELD* fields, uint64_t row_count, uint32_t field_count) : row_count_(row_count), field_count_(field_count), result_(result), fields_(fields)
{
	current_row_ = new field[field_count_];
	assert(current_row_);
}

result_set::~result_set()
{
	clean_up();
}

bool result_set::next_row()
{
	if(!result_)
	{
		return false;
	}

	MYSQL_ROW row = mysql_fetch_row(result_);
	if(!row)
	{
		clean_up();
		return false;
	}

	// 获取字段数
	unsigned long* lengths = mysql_fetch_lengths(result_);
	if(!lengths)
	{
		LOG_WARIN << "sql.sql " << __FUNCTION__ << ": mysql_fetch_lengths, cannot retrieve value lengths. Error " << mysql_error(result->heandle);
		clean_up();
		return false;
	}

	for(uint32_t i = 0; i < field_count_; ++i)
	{
		current_row[i].set_structure_value(row[i], mysql_type_to_field_type(fields[i].type), lengths[i]);
	}
	return true;
}

void result_set::clean_up()
{
	if(current_row_)
	{
		delete[] current_row_;
		current_row_ = nullptr;
	}
	if(result_)
	{
		mysql_free_result(result_);
		result_ = nullptr;
	}
}

const field& result_set::operator[](size_t index) const
{
	assert(index < field_count_);
	return current_row_[index];
}

/* ---------------------------------------- prepare_result_set--------------------------- */

prepare_result_set::prepare_result_set(MYSQL_STMT* stmt, MYSQL_RES* result, uint64_t row_count, uint32_t field_count) : row_count_(row_count), row_position_(0), field_count_(field_count), bind_(NULL), stmt_(stmt), metadata_result_(result)
{
	if(!metadata_result_)
	{
		return;
	}
	if(stmt_->bind_result_done)
	{
		delete[] stmt->bind->length;
		delete[] stmt->bind->is_null;
	}

	bind_ = new MYSQL_BIND[field_count_];
	assert(bind_);

	//对于未来的读者想知道他妈的这个被释放的位置 -  mysql_stmt_bind_result将指针从m_rBind移动到m_stmt-> bind，稍后它被上面的`if（m_stmt-> bind_result_done）`块释放MYSQL_STMT生命周期等于连接生命周期
	my_bool* is_null = new my_bool[field_count_];
	unsigned long* length = new unsigned long[field_count_];
	assert(is_null && length);

	memset(is_null, 0, sizeof(my_bool) * field_count_);
	memset(bind_, 0, sizeof(MYSQL_BIND) * field_count_);
	memset(length, 0, sizeof(unsigned long) * field_count_);

	// 这是我们存储（整个）结果集的地方
	if(mysql_stmt_store_result(stmt_))
	{
		LOG_WARIN << "sql.sql " << __FUNCTION__ << ": mysql_stmt_store_result, cannot bind result from MySQL server. Error: " << mysql_stmt_error(stmt_);
		delete[] bind_;
		delete[] is_null;
		delete[] length;
	}

	row_count_ = mysql_stmt_num_rows(stmt_);

	// 这是我们根据元数据准备缓冲区的地方
	MYSQL_FIELD* field = mysql_fetch_fields(metadata_result_);
	size_t row_size = 0;
	for(uint32_t i = 0; i < field_count_; ++i)
	{
		uint32_t size = size_for_type(&field[i]);
		row_size += size;

		bind_[i].buffer_type = field[i].type;
		bind_[i].buffer_length = size;
		bind_[i].length = &length[i];
		bind_[i].is_null = &is_null[i];
		bind_[i].error = NULL;
		bind_[i].is_unsigned = field[i].flags & UNSIGNED_FLAG;
	}

	char* data_buffer = new char[row_size * row_count_];
	assert(data_buffer);
	for(uint32_t i = 0, offset = 0; i < field_count_; ++i)
	{
		bind_[i].buffer = data_buffer + offset;
		offset += bind_[i].buffer_length;
	}

	// 这是我们将缓冲区绑定到语句的位置
	if(mysql_stmt_bind_result(stmt_, bind_))
	{
		LOG_WARIN << "sql.sql " << __FUNCTION__ << ": mysql_stmt_bind_result, cannot bind result from MySQL server. Error: " << mysql_stmt_error(stmt_);
		mysql_stmt_free_result(stmt_);
		clean_up();
		delete[] is_null;
		delete[] length;
		return;
	}

	rows_.resize(uint32_t(row_count_) * field_count_);
	while(goto_next_row())
	{
		for(uint32_t i = 0; i < field_count_; ++i)
		{
			unsigned long buffer_length = bind_[i].buffer_length;
			unsigned long fetched_length = bind_[i].length;
			if(!bind_[i].is_null)
			{
				void* buffer = stmt_->bind[i].buffer;
				switch(bind_[i].buffer_type)
				{
					case MYSQL_TYPE_TINY_BLOB:
					case MYSQL_TYPE_MEDIUM_BLOB:
					case MYSQL_TYPE_LONG_BLOB:
					case MYSQL_TYPE_BLOB:
					case MYSQL_TYPE_STRING:
					case MYSQL_TYPE_VAR_STRING:
						// 警告 - 当mysql_stmt_fetch返回MYSQL_DATA_TRUNCATED时，如果缓冲区中没有空间，则字符串将不会以空值终止我们不能盲目地终止数据，因为它可能被检索为二进制blob而不是特定字符串在这种情况下使用 Field :: GetCString将导致垃圾.
					//TODO：删除Field :: GetCString并使用boost :: string_ref（目前提议用于TS作为string_view，可能在C ++ 17中）
						if (fetched_length < buffer_length)
						{
							*((char*)buffer + fetched_length) = '\0';
						}
						break;
					default:
						break;
				}

				rows_[uint32_t(row_position_) * field_count_ + i].set_byte_value(buffer, mysql_type_to_field_type(bind_[i].buffer_type), fetched_length);

				//将缓冲区指针移动到下一部分
				stmt_->bind[o].buffer = (char*)buffer + row_size;
			}
			else
			{
				rows_[uint32_t(row_position) * field_count_ + i].set_byte_value(nullptr, mysql_type_to_field_type(bind_[i].buffer_type), *bind_[i].length)
			}
		}
		++row_position_;
	}
	row_position_ = 0;

	// 所有数据都被缓冲，放开mysql c api结构
	mysql_stmt_free_result(stmt_);
}

prepare_result_set::~prepare_result_set()
{
	clean_up();
}

bool prepare_result_set::next_row()
{
	// 只更新m_rowPosition，以便上层代码知道行向量的哪个元素要查看
	if(++row_position_ >= row_count_)
	{
		return false;
	}
	return true;
}

field* prepare_result_set::fetch() const
{
	assert(row_position_ < row_count_);
	return const_cast<field*>(&rows_[uint32_t(row_position_) * field_count_]);
}

const field* prepare_result_set::operator[](size_t index) const
{
	assert(row_position_ < row_count_);
	assert(index < field_count_);
	return rows_[uint32_t(row_position_) * field_count + index];
}

void prepare_result_set::clean_up()
{
	if(metadata_result_)
	{
		mysql_free_result(metadata_result_);
	}
	if(bind_)
	{
		delete[] (char*)bind_->buffer;
		delete[] bind_;
		bind_ = nullptr;
	}
}

bool prepare_result_set::goto_next_row()
{
	// 只在低级代码中调用，即构造函数将迭代每一行数据并缓冲它
	if(row_position_ >= row_count_)
	{
		return false;
	}
	int retval = mysql_stmt_fetch(stmt_);
	return retval == 0 || retval == MYSQL_DATA_TRUNCATED;
}
