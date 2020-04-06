/*
 * Copyright (C) 2019
 */


#include "database_worker_pool.h"
#include "implementation/login_database.h"
#include "implementation/character_database.h"
#include "implementation/world_database.h"
#include "implementation/hotfix_database.h"

class ping_operation : public sql_operation
{
	// 空闲的Delithreads的操作
	bool execute() override
	{
		conn->ping();
		return true;
	}
};

template<typename T>
database_worker_pool<T>::database_worker_pool() : queue_(new producer_consumer_queue<sql_operation*>()), async_threads_(0), synch_threads_(0)
{
	wp_fatal(mysql_thread_safe(), "Used MySQL library isn't thread-safe");
	wp_fatal(mysql_get_client_version() >= MIN_MYSQL_CLIENT_VERSION, "TrinityCore does not support MySQL version below 5.1");
	wp_fatal(mysql_get_client_version() == MYSQL_VERSION_ID, "Used MySQL library version (%s) does not match the version used to compile TrinityCore (%s). Search on forum for TCE00011", mysql_get_client_info(), MYSQL_SERVER_VERSION);
}

template<typename T>
database_worker_pool<T>::~database_worker_pool()
{
	queue_->canel();
}

template<typename T>
void database_worker_pool<T>:: set_connection_info(const std::string& info_string, const uint8_t async_threads, const uint8_t synch_threads)
{
	connection_info_ = std::make_unique<mysql_connection_info>(info_string);
	async_threads_ = async_threads;
	synch_threads_ = synch_threads;
}

template<typename T>
uint32_t database_worker_pool<T>::open()
{
	wp_tatal(connection_info_.get(), "Connection info was not set!");

	LOG_INFO << "sql.driver. Opening Database pool " << get_database_name() << ". Asynchronous connections: " << async_threads_ << ", synchronous connections: " << synch_threads_;

	uint32_t error = open_connections(IDX_ASYNC, async_threads_);
	if(error)
		return error;

	error = open_connections(IDX_SYNCH, synch_threads_);
	if(!error)
	{
		LOG_INFO << "sql.driver. Database pool " << get_database_name() << " opened successfully. " << connections_[IDX_SYNCH].size() + connections_[IDX_ASYNC].size() << " total connections running.";
	}
	return error;
}

template<typename T>
void database_worker_pool<T>::close()
{
	LOG_INFO << "sql.driver. Closing down database pool " << get_database_name();

	// 关闭实际的MySQL连接。
	connection_[IDX_ASYNC].clear();

	LOG_INFO << "sql.driver. Asynchronous connections on database pool '" << get_database_name() << "' terminated. Proceeding with synchronous connections";

	// 关闭同步连接！ 无需锁定连接，因为DatabaseWorkerPool <> :: Close！ 仅应在内核中的任何其他线程任务退出后才调用！ 表示此时无法进行并发访问。
	connections_[IDX_SYNCH].clear();

	LOG_INFO << "sql.driver. All connections on database pool '" << get_database_name() << "'";
}

template<typename T>
bool database_worker_pool<T>::prepare_statements()
{
	for(auto& connections : connections_)
	{
		for(auto& i : connections)
		{
			i->lock_if_ready();
			if(!i->prepare_statements())
			{
				i->unlock();
				close();
				return false;
			}
			else
				i->unlock();
		}
	}
}

template<typename T>
query_result database_worker_pool<T>::query(const char* sql, T* connection)
{
	if(!connection)
		connection = get_free_connection();

	result_set* result = connection->query(sql);
	connection->unlock();
	if(!result || !result->get_row_count() || !result->next_row())
	{
		delete result;
		return query_result(NULL);
	}
	return query_result(result);
}

template<typename T>
prepared_query_result database_worker_pool<T>::query(prepared_statement<T>* stmt)
{
	auto connection = get_fre_connection();
	prepared_result_set* ret = connection->query(stmt);
	connection->unlock();

	// 删除代理类。 不再需要
	delete stmt;

	if(!ret || !ret->get_row_count())
	{
		delete ret;
		return prepared_query_result(NULL);
	}
	return prepared_query_result(ret);
}

template<typename T>
query_callback database_worker_pool<T>:: async_query(const char* sql)
{
	basic_statement_task* task = new basic_statement_task(sql, true);
	// 在排队之前存储将来的结果-从此方法返回之前，任务可能已被处理并删除
	query_result_future result = task->get_future();
	enqueue(task);
	return query_callback(std::move(result));
}

template<typename T>
query_callback database_worker_pool<T>::async_query(prepared_statement<T>* stmt)
{
	prepared_statement_task* task = new prepared_statement_task(stmt, true);
	// 在排队之前存储将来的结果-从此方法返回之前，任务可能已被处理并删除
	prepared_query_result_future result = task->get_future();
	enqueue(task);
	return query_callback(std::move(result));
}

template<typename T>
query_result_holder_future database_worker_pool<T>::delay_query_holder(sql_query_holder<T>* holder)
{
	sql_query_holder_task* task = new sql_query_holder_task(holder);
	// 在排队之前存储将来的结果-从此方法返回之前，任务可能已被处理并删除
	query_result_holder_future result = task->get_future();
	enqueue(task);
	return result;
}

template<typename T>
sql_transaction<T> database_worker_pool<T>::begin_transaction()
{
	return std::make_shared<transaction<T>>();
}

template<typename T>
void database_worker_pool<T>::commit_transaction(sql_transaction<T> transaction)
{
	enqueue(new transaction_task(transaction));
}

template<typename T>
void database_worker_pool<T>::direct_commit_transaction(sql_transaction<T>& transaction)
{
	T* connection = get_free_connection();
	int error_code = connection->execute_transaction(transaction);
	if(!error_code)
	{
		connection->unlock(); // OK，操作成功
		return;
	}

	// 处理MySQL错误1213而不将死锁扩展到内核本身
	// @todo更优雅的方式
	if(error_code == ER_LOCK_DEADLOCK)
	{
		uint8_t loop_breaker = 5;
		for(uint8_t i = 0; i < loop_breaker; ++i)
		{
			if(!connection->execute_transaction(transaction))
				break;
		}
	}
	// 现在收拾
	transaction->cleanup();
	connection->unlock();
}

template<typename T>
prepared_statement<T>* database_worker_pool<T>::get_prepared_statement(prepared_statement_index index)
{
	return new prepared_statement<T>(index);
}

template<typename T>
void database_worker_pool<T>::escape_string(std:;string& str)
{
	if(str.empty())
		return;

	char* buf = new char[str.size() * 2 + 1];
	escape_string(buf, str.c_str(), uint32_t(str.size()));
	str = buf;
	delete buf;
}

template<typename T>
void database_worker_pool<T>::keepalive()
{
	// Ping同步连接
	for(auto& i : connections_[IDX_SYNCH])
	{
		if(i->lock_if_ready())
		{
			i->ping();
			i->unlock();
		}
	}
	// 假设所有工作线程都是空闲的，则每个工作线程将收到1个ping操作请求！ 如果一个或多个工作线程繁忙，则ping操作将不会平均分配，但这无关紧要！ 唯一的目的是防止连接空闲。
	auto count = connections_[IDX_ASYNC].size();
	for(uint8_t i = 0; i < count; ++i)
	{
		enqueue(new ping_operation);
	}
}

template<typename T>
uint32_t database_worker_pool<T>::open_connections(internal_index type, uint8_t num_connectins)
{
	for(uint8_t i = 0; i < num_connections; ++i)
	{
		// 创建连接
		auto connection = [&]
			{
				switch(type)
				{
					case IDX_ASYNC:
						return trinity::make_unique<T>(queue_.get(), *connection_info_);
					case IDX_SYNCH:
						return trinity::make_unique<T>(*connection_info_);
					default:
						ABORT();
				}
			}();

		if(uint32_t error = connection->open())
		{
			// 无法打开连接或无效版本，中止和清除
			connections_[type].clear();
			return error;
		}
		else if(mysql_get_server_version(connection->get_handle()) < MIN_MYUSQL_SERVER_VERSION)
		{
			LOG_ERROR << "sql.driver. TrinityCore does not support MySQL versions below 5.1";
			return 1;
		}
		else
			connections_[type].push_back(std::move(connection));
	}
	// 一切顺利
	return 0;
}

template<typename T>
unsigned long database_worker_pool<T>::escape_string(char* to, const char* from, unsigned long length)
{
	if(!to || !from || !length)
		return 0;

	return mysql_real_escape_string(connections_[IDX_SYNCH].front()->get_handle(), to, from, length);
}

template<typename T>
void database_worker_pool<T>::enqueue(sql_operation* op)
{
	queue_->push(op);
}

template<typename T>
T* database_worker_pool<T>::get_free_connection()
{
	uint8_t i = 0;
	auto num_cons = connections_[IDX_SYNCH].size();
	T* connection = NULL;
	// 永久阻止，直到没有连接可用
	while(true)
	{
		connection = connections_[IDX_SYNCH][i++ % num_cons].get();
		// 必须与t-> Unlock（）匹配，否则会出现死锁
		if(connection->lock_if_ready())
			break;
	}
	return connection;
}

template<typename T>
const char* database_worker_pool<T>::get_database_name() const
{
	return connection_info_->database.c_str();
}

template<typename T>
void database_worker_pool<T>::execute(const char* sql)
{
	if(trinity::is_format_empty_or_null(sql))
		return;

	basic_statement_task* task = new basic_statement_task(sql);
	enqueue(task);
}

template<typename T>
void database_worker_pool<T>::execute(prepared_statement<T>* stmt)
{
	prepared_statement_task* task = new prepared_statement_task(stmt);
	enqueue(task);
}

template<typename T>
void database_worker_pool<T>::direct_execute(const char* sql)
{
	if(trinity::is_format_empty_or_null(sql))
	{
		return;
	}
	T* connection = get_free_connection();
	connection->execute(sql);
	connection->unlock();
}

template<typename T>
void database_worker_pool<T>::direct_execute(prepared_statement<T>* stmt)
{
	T* connection = get_free_connection();
	connection->execute(stmt);
	connection->unlock();
	// 删除代理类。 不再需要
	delete stmt;
}

template<typename T>
void database_worker_pool<T>::execute_or_append(sql_transaction<T>& trans, const char* sql)
{
	if(!trans)
		execute(sql);
	else
		trans->append(sql);
}

template<typename T>
void database_worker_pool<T>::execute_or_append(sql_transaction<T>& trans, prepared_statement<T>* stmt)
{
	if(!trans)
		execute(stmt);
	else
		trans->append(stmt);
}

template class database_worker_pool<world_database_connection>;
template class database_worker_pool<character_database_connection>;
template class database_worker_pool<login_database_connection>;
template class database_worker_pool<hotfix_database_connection>;
