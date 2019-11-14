/*
 * Copyright (C) 2018
 */

#include <cassert>

#include "log_worker.h"
#include "log_message.h"

log_worker::log_worker() : running_(false), current_buffer_(new Buffer), next_buffer_(new Buffer),
	buffers_()
{
	buffers_.reserve(16);
}

void log_worker::add_log_task(const log_task& task)
{
	std::lock_guard<std::mutex> lock(mutex_);

	if (current_buffer_->avail()) // 当前缓存有空间
		current_buffer_->push(task);
	else
	{
		// 将当前已满的缓存放到buffers里
		buffers_.emplace_back(current_buffer_.release());

		if (next_buffer_) // 有二级缓存
		{
			// 当前缓存指向二级缓存
			current_buffer_ = std::move(next_buffer_);
		}
		else
		{
			// 申请新的缓存
			current_buffer_.reset(new Buffer); // Rarely happens
		}
		current_buffer_->push(task);
		cond_.notify_one();
	}
}

void log_worker::thread_func()
{
	assert(running_ == true);

	// 2个缓存
	Buffer_ptr new_buffer1(new Buffer);
	Buffer_ptr new_buffer2(new Buffer);

	// 将要写的缓存
	Buffer_vector buffers_to_write;
	buffers_to_write.reserve(16);

	while (running_)
	{
		assert(new_buffer1 && new_buffer1->size() == 0);
		assert(new_buffer2 && new_buffer2->size() == 0);
		assert(buffers_to_write.empty());

		{

      		if (buffers_.empty())  // unusual usage!
      		{
				std::unique_lock<std::mutex> lock(mutex_);
				cond_.wait_for(lock, std::chrono::seconds(flush_interval_),
						[&]{ return !buffers_.empty(); });
      		}

			// 无论current_buffer_满还是不满，都天骄到buffers里
			buffers_.emplace_back(current_buffer_.release());

			// current_buffer_指向新的缓存
			current_buffer_ = std::move(new_buffer1);

			// 将buffers_交换到要写的缓存里
			buffers_to_write.swap(buffers_);
			if (!next_buffer_)
			{
				// next_buffer_指向二级缓存
				next_buffer_ = std::move(new_buffer2);
			}
		}

		assert(!buffers_to_write.empty());

		if (buffers_to_write.size() > 25)
		{
			char buf[256];
			snprintf(buf, sizeof buf, "Dropped log task at %s, %zd larger buffers\n", 
					log_message::get_time_str(time(NULL)).c_str(), 
					buffers_to_write.size() - 2);
			fputs(buf, stderr);
			buffers_to_write.erase(buffers_to_write.begin() + 2, 
					buffers_to_write.end());
		}

		for(auto& log_task : buffers_to_write)
		{
			// FIXME: use unbuffered stdio FILE ? or use ::writev ?
			log_task.execute();
		}

		if (buffers_to_write.size() > 2)
		{
			// drop non-bzero-ed buffers, avoid trashing
			buffers_to_write.resize(2);
		}

		if (!new_buffer1)
		{
			assert(!buffers_to_write.empty());
			new_buffer1 = std::move(buffers_to_write.back());
			buffers_to_write.pop_back();
			new_buffer1->reset();
		}

		if (!new_buffer2)
		{
			assert(!buffers_to_write.empty());
			new_buffer2 = std::move(buffers_to_write.back());
			buffers_to_write.pop_back();
			new_buffer2->reset();
		}

		buffers_to_write.clear();
	}
}

log_worker* log_worker::instance()
{
	static log_worker instance;
	return &instance;
}
