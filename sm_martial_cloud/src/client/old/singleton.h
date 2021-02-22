/*
 * Copyright (C) 2018
 */

#ifndef SINGLETON_H
#define SINGLETON_H

#include <mutex>

template<typename T>
class Singleton
{
	public:
		static T* get_instance()
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

		static T* singleton_;
		std::mutex m_;
};

template<typename T>
T* Singleton<T>::singletion_ = nullptr;

#endif
