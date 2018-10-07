/*
 * Copyright (C) 2018
 */

#include "byte_buffer.h"

Byte_buffer::Byte_buffer(size_t size) : read_pos_(0), write_pos_(0)
{
        // 必须初始化，未初始化的情况下，gcc下无论storages_.reserve()预留很多空间，使用memcpy依然无效
        // 或者使用resize()
	storages_.assign(size, 0);      
}

Byte_buffer::Byte_buffer(const Byte_buffer& buf) : read_pos_(buf.read_pos_),
	write_pos_(buf.write_pos_), storages_(buf.storages_) {}

Byte_buffer& Byte_buffer::operator=(const Byte_buffer& buf)
{
	read_pos_ = buf.read_pos_;
	write_pos_ = buf.write_pos_;
	storages_ = buf.storages_;
}

Byte_buffer::Byte_buffer(Byte_buffer&& buf): 
	read_pos_(std::move(buf.read_pos_)),
	write_pos_(std::move(buf.write_pos_)),
	storages_(std::move(buf.storages_)) {}

Byte_buffer& Byte_buffer::operator=(Byte_buffer&& buf)
{
	read_pos_ = std::move(buf.read_pos_);
	write_pos_ = std::move(buf.write_pos_);
	storages_ = std::move(buf.storages_);
}


Byte_buffer& Byte_buffer::operator<<(const int32_t& val)
{
	push(reinterpret_cast<const char*>(&val), sizeof(int32_t));
	return *this;
}

Byte_buffer& Byte_buffer::operator<<(const std::string& str)
{
	if(!str.empty())
		push(str.c_str(), str.size());
	return *this;
}

Byte_buffer& Byte_buffer::operator<<(const char* str)
{
	if(str)
		push(str, strlen(str));
	return *this;
}

void Byte_buffer::push(const char* src, size_t cnt)
{
	if(!src || !cnt) return;

	if(storages_.size() < write_pos_ + cnt)
		storages_.resize(write_pos_ + cnt);

	memcpy(&storages_[write_pos_], src, cnt);
	write_pos_ += cnt;
}

Byte_buffer& Byte_buffer::operator>>(int32_t& value)
{
	value = get<int32_t>();
	return *this;
}

Byte_buffer& Byte_buffer::operator>>(std::string& str)
{
	str.clear();
	while(read_pos_ < storages_.size())
	{
		char c = get<char>();
		if(c == 0)
			break;
		str += c;
	}
	return *this;
}

Byte_buffer& Byte_buffer::operator>>(char str[])
{
	std::string s;
	while(read_pos_ < storages_.size())
	{
		char c = get<char>();
		if(c == 0)
			break;
		s += c;
	}
	strncpy(str, s.c_str(), s.size());
	return *this;
}

// 注意char*字符串可能包含\0
Byte_buffer::operator char*()
{
	return const_cast<char*>(storages_.data());
}

void Byte_buffer::clear() { read_pos_ = write_pos_ = 0; storages_.clear(); }
