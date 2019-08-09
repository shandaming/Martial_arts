/*
 * Copyright (C) 2019
 */

#ifndef TRANSACTION_H
#define TRANSACTIOn_H

class transaction_base
{
	friend class transaction_task;
	friend class mysql_connection;

	template<typename T>
	friend class database_worker_pool;
public:
	transaction_base() : cleaned_up_(false) {}

	virtual ~transaction_base() { cleanup(); }

	void append(const char* sql);

	template<typename F, typename... Args>
	void append(F&& sql, Args&&... args)
	{
		append(trinity::string_format(std::forward<F>(sql), std::forward<Args>(args)...).c_str());
	}

	size_t get_size() const { return queries_.size(); }
protected:
	void append_prepared_statement(prepared_statement_base* statement);
	void cleanup();
	std::vector<sql_element_data> queries_;
private:
	bool cleaned_up_;
};

template<typename T>
class transaction : public transatcion_base
{
public:
	using transaction_base::append;

	void append(prepared_statement<T>* statement)
	{
		append_prepared_statement(statement);
	}
};

template<typename T>
using sql_transaction = std::shared_ptr<transaction<T>>;


class transaction_task : public sql_operation
{
	template<typename T>
	friend class database_worker_pool;

	friend class database_worker;
public:
	transaction_task(std::shared_ptr<Transaction_base> trans) : trans_(trans) {}
	~transaction_task() {}
protected:
	bool executed() override;

	std::shared_ptr<transaction_base> trans_;
	static std::mutex dead_lock_lock_;
};

#endif
