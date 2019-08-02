/*
 * Copyright (C) 2019
 */

#ifndef QUERY_CALLBACK_H
#define QUERY_CALLBACK_H

class query_callback
{
public:
	explicit query_callback(query_result_future&& result);
	explicit query_callback(prepared_query_result_future&& result);

	query_callback(const query_callback&) = delete;
	query_callback& operator=(const query_callback&) = delete;

	query_callback(query_callback&& right);
	query_callback& operator=(query_callback&& right);

	~query_callback();

	query_callback&& with_callback(std::function<void(query_result)>&& callback);
	query_callback&& with_prepared_callback(std::function<void(prepared_query_result)>&& callback);

	query_callback&& with_chaining_callback(std::function<void(query_callback&, query_result)>&& callback);
	query_callback&& with_chaining_prepared_callback(std::function<void(query_callback&, prepared_query_result)>&& callback);

	//
	void set_next_query(query_callback&& next);

	enum status
	{
		not_ready,
		next_step,
		completed
	};

	status invoke_if_ready();
private:
	template<typename T>
	friend void construct_active_member(T* obj);

	template<typename T>
	friend void destroy_active_member(T* obj);

	template<typename T>
	friend void move_from(T* to, T&& from);

	union
	{
		query_result_future string;
		prepared_query_result_future prepared;
	};

	bool is_prepared_;

	std::unique_ptr<query_callback_data, std::list<query_callback_data>> callbacks_;
};

#endif
