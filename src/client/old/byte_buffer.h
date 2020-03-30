/*
 * Copyright (C) 2018
 */

#ifndef BYTE_BUFFER_H
#define BYTE_BUFFER_H

#include <string>
#include <cstring>
#include <cassert>

class Byte_buffer
{
	public:
		Byte_buffer(size_t size = 8192);

		Byte_buffer(const Byte_buffer& buf);
		Byte_buffer& operator=(const Byte_buffer& buf);

		Byte_buffer(Byte_buffer&& buf);
		Byte_buffer& operator=(Byte_buffer&& buf);

		Byte_buffer& operator<<(const int32_t& val);
		Byte_buffer& operator<<(const std::string& str);
		Byte_buffer& operator<<(const char* str);

		// 向storages_里写数据
		void push(const char* src, size_t cnt);

		Byte_buffer& operator>>(int32_t& value);
		Byte_buffer& operator>>(std::string& str);
		Byte_buffer& operator>>(char str[]);

		template<typename T>
		T get()
		{ 
			T res = get<T>(read_pos_); 
			read_pos_ += sizeof(T); 
			return res;
		}

		template<typename T>
		T get(size_t pos) const
		{
			assert(pos + sizeof(T) <= storages_.size());
			return *(reinterpret_cast<T const*>(&storages_[pos]));
		}

		operator char*();
                int size() const { return write_pos_; }
		void clear();
	private:
		size_t read_pos_;
		size_t write_pos_;
		std::string storages_;
};

#endif
