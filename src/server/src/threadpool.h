/*
 * Copyright (C) 2018
 */

#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <memory>
#include <functional>

class Threadpool
{
	public:
		Threadpool() : stop_(false)
		{
			const int n = std::thread::hardware_concurrency();
			for(int i = 0; i < n; ++i)
			{
				workers_.emplace_back(
						[this]
						{
							for(;;)
							{
								std::function<void()> task;

								std::unique_lock<std::mutex> lock(mutex_);
								this->condition_.wait(lock, 
										[this] {return this->stop_ || 
										!this->tasks_.empty();});
								if(this->stop_ && this->tasks_.empty())
									return;
								task = std::move(this->tasks_.front());
								this->tasks_.pop();
							        task();
							}
						});
			}
		}

		~Threadpool()
		{
			std::unique_lock<std::mutex> lock(mutex_);
			stop_ = true;

			condition_.notify_all();
			for(std::thread& w : workers_)
				w.join();
		}

		template<typename F, typename... Args>
		auto enqueue(F&& f, Args&&... args)->
			std::future<typename std::result_of<F(Args...)>::type>
		{
			using Return_type = typename std::result_of<F(Args...)>::type;

			auto task = std::make_shared<std::packaged_task<Return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

			std::future<Return_type> res = task->get_future();
			std::unique_lock<std::mutex> lock(mutex_);
			if(stop_)
				throw "Enqueue on stopped threadpool";
			tasks_.emplace([task]() { (*task)(); });

			condition_.notify_one();
			return res;
		}
	private:
		std::vector<std::thread> workers_;
		std::queue<std::function<void()>> tasks_;

		std::mutex mutex_;
		std::condition_variable condition_;
		bool stop_;
};

#endif
