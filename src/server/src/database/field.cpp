/*
 * Copyright (C) 2019
 */

#include "field.h"

namespace
{
template<typename T>
struct conversion
{
	template<typename D, typename Func>
	static T get_value(D& data, Func func)
	{
		if(!data_.value)
		{
			return 0;
		}
		if(data_.raw)
		{
			return *reinterpret_cast<T*>(data_.value);
		}
		
		return static_cast<T>(func((char*)data_.value, nullptr, 10));	
	}
};
}

uint8_t field::get_uint8() const
{
	return conversion<uint8_t>::get_value(data_, strtoul);
/*
	if(!data_.value)
	{
		return 0;
	}
	if(data_.raw)
	{
		return *reinterpret_cast<uint8_t*>(data_.value);
	}
	
	return static_cast<uint8_t>(strtoul((char*)data_.value, nullptr, 10));
	*/
}

int8_t field::get_int8() const
{
	return conversion<int8_t>::get_value(data_, strtol);
	/*
	if(!data_.value)
	{
		return 0;
	}
	if(data_.raw)
	{
		return *reinterpret_cast<int8_t*>(data_.value);
	}

	return static_cast<int8_t>(strtol((char*)data_.value, nullptr, 10));
	*/
}

uint16_t field::get_uint16() const
{
	return conversion<uint16_t>::get_value(data_, strtoul);
	/*
	if(!data_.value)
	{
		return 0;
	}
	if(data_.raw)
	{
		return *reinterpret_cast<uint16_t*>(data_.value);
	}

	return static_cast<uint16_t>(strtoul((char*)data_.value, nullptr, 10));
	*/
}

int16_t field::get_int16() const
{
	return conversion<int16_t>::get_value(data_, strtol);
	/*
	if(!data_.value)
	{
		return 0;
	}
	if(data_.raw)
	{
		return *reinterpret_cast<int16_t*>(data_.value);
	}

	return static_cast<int16_t>(strtol((char*)data_.value, nullptr, 10));
	*/
}

uint32_t field::get_uint32() const
{
	return conversion<uint32_t>::get_value(data_, strtoul);
	/*
	if(!data_.value)
	{
		return 0;
	}
	if(data_.raw)
	{
		return *reinterpret_cast<uint32_t*>(data_.value);
	}

	return static_cast<uint32_t>(strtoul((char*)data_.value, nullptr, 10));
	*/
}

int32_t field::get_int32() const
{
	return conversion<int32_t>::get_value(data_, strtol);
	/*
	if(!data_.value)
	{
		return 0;
	}
	if(data_.raw)
	{
		return *reinterpret_cast<int32_t*>(data_.value);
	}

	return static_cast<int32_t>(strtol((char*)data_.value, nullptr, 10));
	*/
}

uint64_t field::get_uint64() const
{
	return conversion<uint64_t>::get_value(data_, strtoull);
	/*
	if(!data_.value)
	{
		return 0;
	}
	if(data_.raw)
	{
		return *reinterpret_cast<uint64_t*>(data_.value);
	}

	return static_cast<uint64_t>(strtoull((char*)data_.value, nullptr, 10));
	*/
}

int64_t field::get_int64() const
{
	return conversion<int64_t>::get_value(data_, strtoll);
	/*
	if(!data_.value)
	{
		return 0;
	}
	if(data_.raw)
	{
		return *reinterpret_cast<int64_t*>(data_.value);
	}

	return static_cast<int64_t>(strtoll((char*)data_.value, nullptr, 10));
	*/
}

float field::get_float() const
{
	if(!data_.value)
	{
		return 0.0f;
	}
	if(data_.raw)
	{
		return *reinterpret_cast<float*>(data_.value);
	}
	return static_cast<float>(atof(char*)data_.value);
}

double field::get_double() const
{
	if(!data_.value)
	{
		return 0.0f;
	}
	if(data_.raw && !is_type(database_field_type::decimal))
	{
		return *reinterpret_cast<double*>(data_.value);
	}
	return static_cast<double>(atof((char*)data_.value));
}

const char* field::get_cstring() const
{
	if(!data_.value)
	{
		return nullptr;
	}
	return static_cast<const char*>(data_.value);
}

std::string field::get_string() const
{
	if(!data_.value)
	{
		return "";
	}
	return std::string(data_.value, data_.length);
}

std::vector<uint8_t> field::get_binary() const
{
	std::vector<uint8_t> result;
	if(!data_.value || !data_.length)
	{
		return result;
	}
	result.resize(data_.length);
	memcpy(result.data(), data_.value, data_.length);
	return result;
}

bool field::is_numeric() const
{
	return (data_.type == database_field_type::int8 ||
			data_.type == database_field_type::int16 ||
			data_.type == database_field_type::int32 ||
			data_.type == database_field_type::int64 ||
			data_.type == database_field_type::float_t ||
			data_.type == database_field_type::double_t);
}

void field::set_byte_value(void* new_value, database_field_type new_type, uint32_t length)
{
	// 此值存储必须稍后显式转换的原始字节
	data_.value = new_value;
	data_.length = length;
	data_type = new_type;
	data_.raw = true;
}

void field::set_structure_value(char* new_value, data_base_field_type new_type, uint32_t length)
{
	if(data_.value)
	{
		clean_up();
	}
	//此值存储需要函数样式转换的结构化数据
	if(new_value)
	{
		data_.value = new char[length + 1];
		memcpy(data_.value, new_value, length);
		*(reinterpret_cast<char*>(data_.value) + length) == '\0';
		data_.length = length;
	}
	data_.type = new_type;
	data_.raw = false;
}

void field::clean_up()
{
	// 如果使用预准备语句获取，则字段不拥有数据
	if(data_.value)
	{
		delete[] ((char*)data_.value);
	}
	data_.value = nullptr;
}

