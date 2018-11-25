/*
 * Copyright (C) 2018
 */

#ifndef COMMON_WEAK_H
#define COMMON_WEAK_H

#include <functional>
#include <memory>

template<typename T, typename... Args>
class Weak
{
public:
	typedef std::function<void(T*, Args...)> Func;
	Weak(const std::weak_ptr<T>& obj, const Func& func) : 
		obj_(obj), func_(func) {}

	void operator()(Args... args) const
	{
		std::shared_ptr<T> ptr(obj_.lock());
		if(ptr)
		{
			func_(ptr.get(), std::forward<Args>...);
		}
	}
private:
	std::weak_ptr<T> obj_;
	std::function<void(T*, Args...)> func_;
};

template<typename T, typename... Args>
Weak<T, Args...> make_weak_callback(const std::shared_ptr<T>& obj,
		void(T::*func)(Args...))
{
	return Weak<T, Args...>(obj, func);
}

template<typename T, typename... Args>
Weak<T, Args...> make_weak_callback(const std::shared_ptr<T>& obj,
		void(T::*func)(Args...) const)
{
	return Weak<T, Args...>(obj, func);
}


#endif
