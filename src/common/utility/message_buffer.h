/*
 * Copyright (C) 2019
 */

#ifndef MESSAGE_BUFFER_H
#define MESSAGE_BUFFER_H

#include <vector>
#include <cstring>

class message_buffer
{
	typedef std::vector<uint8_t>::size_type size_type;
public:
	message_buffer() : wpos_(0), rpos_(0), storage_()
	{
		storage_.resize(4096);
	}

	explicit message_buffer(std::size_t initial_size) : wpos_(0), rpos_(0), storage_()
	{
		storage_.resize(initial_size);
	}

	message_buffer(const message_buffer& right) : wpos_(right.wpos_), rpos_(right.rpos_), storage_(right.storage_) {}

	message_buffer& operator=(const message_buffer& right)
	{
		if(this != &right)
		{
			wpos_ = right.wpos_;
			rpos_ = right.rpos_;
			storage_ = right.storage_;
		}
		return *this;
	}

	message_buffer(message_buffer&& right) : wpos_(right.wpos_), rpos_(right.rpos_), storage_(right.move()) {}

	message_buffer& operator=(message_buffer&& right)
	{
		if(this != &right)
		{
			wpos_ = right.wpos_;
			rpos_ = right.rpos_;
			storage_ = right.move();
		}
		return *this;
	}

	void reset()
	{
		wpos_ = 0;
		rpos_ = 0;
	}

	void resize(size_type bytes) { storage_.resize(bytes); }

	uint8_t* get_base_pointer() { return storage_.data(); }

	uint8_t* get_read_pointer() { return get_base_pointer() + rpos_; }

	uint8_t* get_write_pointer() { return get_base_pointer() + wpos_; }

	void read_completed(size_type bytes) { rpos_ += bytes; }

	void write_completed(size_type bytes) { wpos_ += bytes; }

	size_type get_active_size() const { return wpos_ - rpos_; }

	size_type get_remaining_space() const { return storage_.size() - wpos_; }

	size_type get_buffer_size() const { return storage_.size(); }

	// 丢弃无效数据
	void normalize()
	{
		if(rpos_)
		{
			if(rpos_ != wpos_)
				std::memmove(get_base_pointer(), get_read_pointer(), get_active_size());
			wpos_ -= rpos_;
			rpos_ = 0;
		}
	}

	// 确保有“一些”可用空间，请确保在此之前调用Normalize（）
	void ensure_free_space()
	{
		// 调整缓冲区大小（如果已满）
		if(get_remaining_space() == 0)
			storage_.resize(storage_.size() * 3 / 2);
	}

	void write(const void* data, std::size_t size)
	{
		if(size)
		{
			memcpy(get_write_pointer(), data, size);
			write_completed(size);
		}
	}

	std::vector<uint8_t>&& move()
	{
		wpos_ = 0;
		rpos_ = 0;
		return std::move(storage_);
	}
private:
	size_type wpos_;
	size_type rpos_;
	std::vector<uint8_t> storage_;
};

#endif
