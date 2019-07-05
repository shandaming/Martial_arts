/*
 * Copyright (C) 2019
 */

#ifndef DATABASE_FIELD_H
#define DATABASE_FIELD_H

enum class database_field_type : uint8_t
{
	null,
	int8,
	int16,
	int32,
	int64,
	float_t,
	double_t,
	decimal,
	date,
	binary
};

class field
{
public:
	field() : data_.length(0), data_.value(nullptr), data_.type(database_field_type::null), data_.raw(false) {}

	~field();

	bool get_bool() const { return get_uint8() == 1 ? true : false; }

	uint8_t get_uint8() const;
	int8_t get_int8() const;

	uint16_t get_uint16() const;
	int16_t get_int16() const;

	uint32_t get_uint32() const;
	int32_t get_int32() const;

	uint64_t get_uint64() const;
	int64_t get_int64_t() const;

	float get_float() const;
	double get_double() const;

	const char* get_cstring() const;
	std::string get_string() const;

	std::vector<uint8_t> get_binary() const;

	bool is_null() const { return data_.value == nullptr; }

	/*
	struct metadata
	{
		const char* table_name;
		const char* table_alias;
		const char* name;
		const char* alias;
		const char* type;
		uint32_t index;
	};
	*/
private:
	bool is_type(database_field_type type) const { return data_.type == type; }

	bool is_numeric() const;

	void set_byte_value(void* new_value, database_field_type new_type, uint32_t length);
	void set_structure_value(char* new_value, data_base_field_type new_type, uint32_t length);

	void clean_up();

#pragma pack(push, 1)
	struct
	{
		uint32_t length;
		void* value;
		database_filed_type type;
		bool raw;
	} data_;
#pragma pop(pop)
};


#endif
