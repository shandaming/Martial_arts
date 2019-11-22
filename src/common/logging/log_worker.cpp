/*
 * Copyright (C) 2018
 */

#include <cassert>

#include "log_worker.h"
#include "log_message.h"

log_worker::log_worker() : is_working_(false), current_task_list_(new task_list), next_task_list_(new task_list),
	tasks_list_()
{
	tasks_list_.reserve(16);
}

log_worker::~log_worker()
{
	if (is_working_)
		stop();
}

void log_worker::add_task(const log_task& task)
{
	std::lock_guard<std::mutex> lock(mutex_);

	if (current_task_list_->avail()) // 当前缓存有空间
		current_task_list_->push(task);
	else
	{
		// 将当前已满的缓存放到buffers里
		tasks_list_.emplace_back(current_task_list_.release());

		if (next_task_list_) // 有二级缓存
		{
			// 当前缓存指向二级缓存
			current_task_list_ = std::move(next_task_list_);
		}
		else
		{
			// 申请新的缓存
			current_task_list_.reset(new task_list); // Rarely happens
		}
		current_task_list_->push(task);
		cond_.notify_one();
	}
}

void log_worker::working()
{
	is_working_ = true;
	thread_ = std::make_unique<std::thread>([this]{ this->do_work(); });
}

void log_worker::stop()
{
	is_working_ = false;
	cond_.notify_one();
	thread_->join();
}

void log_worker::do_work()
{
	assert(is_working_ == true);

	// 2个缓存
	task_list_ptr new_task_list1(new task_list);
	task_list_ptr new_task_list2(new task_list);

	// 将要写的缓存
	tasks_list_vector tasks_list_to_write;
	tasks_list_to_write.reserve(16);

	while (is_working_)
	{
		assert(new_task_list1 && new_task_list1->size() == 0);
		assert(new_task_list2 && new_task_list2->size() == 0);
		assert(tasks_list_to_write.empty());

		{

      		if (tasks_list_.empty())  // unusual usage!
      		{
				std::unique_lock<std::mutex> lock(mutex_);
				cond_.wait_for(lock, std::chrono::seconds(flush_interval_),
						[&]{ return !tasks_list_.empty(); });
      		}

			// 无论current_buffer_满还是不满，都天骄到buffers里
			tasks_list_.emplace_back(current_task_list_.release());

			// current_buffer_指向新的缓存
			current_task_list_ = std::move(new_task_list1);

			// 将buffers_交换到要写的缓存里
			tasks_list_to_write.swap(tasks_list_);
			if (!next_task_list_)
			{
				// next_buffer_指向二级缓存
				next_task_list_ = std::move(new_task_list2);
			}
		}

		assert(!tasks_list_to_write.empty());

		if (tasks_list_to_write.size() > 25)
		{
			char buf[256];
			snprintf(buf, sizeof buf, "Dropped log task at %s, %zd larger buffers\n", 
					log_message::get_time_str(time(NULL)).c_str(), 
					tasks_list_to_write.size() - 2);
			fputs(buf, stderr);
			tasks_list_to_write.erase(tasks_list_to_write.begin() + 2, 
					tasks_list_to_write.end());
		}

		for(auto& log_task : tasks_list_to_write)
		{
			// FIXME: use unbuffered stdio FILE ? or use ::writev ?
			log_task->execute();
		}

		if (tasks_list_to_write.size() > 2)
		{
			// drop non-bzero-ed buffers, avoid trashing
			tasks_list_to_write.resize(2);
		}

		if (!new_task_list1)
		{
			assert(!tasks_list_to_write.empty());
			new_task_list1 = std::move(tasks_list_to_write.back());
			tasks_list_to_write.pop_back();
			//new_task_list1->reset();
		}

		if (!new_task_list2)
		{
			assert(!tasks_list_to_write.empty());
			new_task_list2 = std::move(tasks_list_to_write.back());
			tasks_list_to_write.pop_back();
			//new_task_list2->reset();
		}

		tasks_list_to_write.clear();
	}
}
