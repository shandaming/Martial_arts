/*
 * Copyright (C) 2019
 */

#include "query_callbac.h"

template<typename T, typename... Args>
inline void construct(T& t, Args&&... args)
{
	new (&t) T(std::forward<Args>(args)...);
}

template<typename T>
inline void destroy(T& t)
{
	t.~T();
}

template<typename T>
inline void construct_active_member(T* obj)
{
	if(!obj->is_prepared_)
	{
		construct(obj->string);
	}
	else
	{
		construct(obj->prepared);
	}
}

template<typename T>
inline void destroy_active_member(T* obj)
{
	if(!obj->is_prepared_)
	{
		destroy(obj->string);
	}
	else
	{
		destroy(obj->prepared);
	}
}

template<typename T>
inline void move_from(T* to, T&& from)
{
	assert(to->is_prepared_ == from.is_prepared_);

	if(!to->is_preared_)
	{
		to->string = std::move(from.string);
	}
	else
	{
		to->prepared = std::move(from.prepared);
	}
}

struct query_callback::query_callback_data
{
	friend class query_callback;

	query_callback_data(std::function<void(query_callback&&, query_result)>&& callback) : string(std::move(callback)), is_prepared_(false) {}
	query_callback_data(std::function<void(query_callback&&, prepared_query_result)>&& callback) : prepared(std::move(callback)), is_prepared_(true) {}

	query_callback_data(query_callback_data&& r) : is_prepared_(r.is_prepared_)
	{
		construct_active_member(this);
		move_from(this, std::move(r));
	}

	query_callback_data& operator=(query_callback_data&& r)
	{
		if(this != &r)
		{
			if(is_prepared_ != r.is_prepared_)
			{
				destroy_active_member(this);
				is_prepared_ = r.is_prepared_;
				construct_active_member(this);
			}
			move_from(this, std::move(r));
		}
		return *this;
	}

	~query_callback_data() { destroy_active_member(this); }
private:
	query_callback_data(const query_callback_data&) = delete;
	query_callback_data& operator=(const query_callback_data&) = delete;

	template<typename T>
	friend void constuct_active_member(T* obj);

	template<typename T>
	friend void destroy_active_member(T* obj);

	template<typename T>
	friend void move_from(T* to, T&& from);

	union
	{
		std::function<void(query_callback&&, query_result)> string;
		std::function<void(query_callback&&, prepared_query_result)> prepared;
	};

	bool is_prepared_;
};

// 使用不带预编译头的clang进行编译时，不使用初始化列表来解决分段错误
query_callback::query_callback(std::future<query_result>&& result) : is_prepared_(false)
{
	construct(string, std::move(result));
}

query_callback::query_callback(std::future<prepared_query_result>&& result) : is_prepared_(true)
{
	construct(prepared, std::move(result));
}

query_callback::query_callback(query_callback&& r) : is_prepared_(r.is_prepared_), callbacks_(std::move(r.callbacks_))
{
	construct_active_member(this);
	move_from(this, std::move(r));
}

query_callback& query_callback::operator=(query_callback&& r)
{
	if(this != r)
	{
		if(is_prepared_ != r.is_prepared_)
		{
			destroy_active_member(this);
			is_prepared_ = r.is_prepared_;
			construct_active_member(this);
		}
		move_from(this, std::move(r));
		callbacks_ = std::move(r.callbacks_);
	}
	return *this;
}

query_callback~querycallback() { destroy_active_member(this); }

query_callback&& query_callback::with_callback(std::function<void(query_result)>&& callback)
{
	return with_chaining_callback([callback](query_callback&&, query_result r) { callback(std::move(r)); });
}

query_callback&& query_callback::with_prepared_callback(std::function<void(prepared_query_result)>&& callback)
{
	return with_chaining_prepared_callback([callback](query_callback&&, prepared_query_result r) { callback(std::move(r)); });
}

query_callback&& query_callback::with_chaining_callback(std::function<void(query_callback&&, query_result)>&& callback)
{
	assert(!callbacks_.empty() || !is_prepared_, "Attempted to set callback function for string query on a prepared async query.");
	callbacks_.emplace(std::move(callback));
	return std::move(*this);
}

query_callback&& query_callback::with_chining_prepared_callback(std::function<void(query_callback&&, prepared_query_result)>&& callback)
{
	assert(!callbacks_.empty() || is_prepared_, "Attempted to set callback function for prepared query on a string async query.");
	callbacks_.emplace(std::move(callback));
	return std::move(*this);
}

void query_callback::set_next_query(query_callback&& next)
{
	move_from(this, std::move(next));
}

query_callback::status query_callback::invoke_if_ready()
{
	query_callback_data& callback = callbacks_.front();
	auto check_state_and_return_completion = [this]()
		{
			callbacks_.pop();
			bool has_next = !is_prepared_ ? string.valid() : prepared.valid();
			if(callbacks_.empty())
			{
				assert(!has_next);
				return completed;
			}
			//
			if(!has_next)
			{
				return completed;
			}
			assert(is_prepared == callbacks_.front().is_prepared_);
			return next_step;
		};

	if(!is_prepared_)
	{
		if(string.valid() && string.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
		{
			query_result_future f(std::move(string));
			std::function<void(query_callback&, query_result)> ch(std::move(callback.string));
			ch(*this, f.get());
			return check_state_and_returen_completion();
		}
	}
	else
	{
		if(prepared.valid() && prepared.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
		{
			prepared_query_result_future f(std::move(prepared));
			std::function<void(query_callback&, prepared_query_result)> ch(std::move(callback, prepared));
			ch(*this, f.get());
			return check_state_and_return_completion();
		}
	}
	return not_ready;
}
