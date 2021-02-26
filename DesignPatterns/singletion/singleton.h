/*
 * Copyright (C) 2018
 */

#ifndef SINGLETON_H
#define SINGLETON_H

#include <mutex>

class Singleton
{
	public:
		static Singleton* get_instance()
		{
			if(singleton_ == nullptr)
			{
				std::scoped_lock<std::mutex> lock(m_);
				if(singleton_ == nullptr)
					singleton = new Singleton();
			}
			return singleton_;
		}
	private:
		Singleton() = default;

		static Singleton* singleton_;
		std::mutex m_;
};

#endif
