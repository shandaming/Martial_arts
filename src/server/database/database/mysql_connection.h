/*
 * Copyright (C) 2019
 */

#ifndef DATABASE_MYSQL_CONNECTION_H
#define DATABASE_MYSQL_CONNECTION_H

#include <string>
#include <map>
#include <utility>

#include "query_result.h"
#include "transaction.h"
#include "database_worker.h"
#include "producer_consumer_queue.h"

enum connection_flags
{
	CONNECTION_ASYNC = 0x01,
	CONNECTION_SYNCH = 0x02,
	CONNECTION_BOTH = CONNECTION_ASYNC | CONNECTION_SYNCH
};

struct mysql_connection_info
{
	explicit mysql_connection_info(const std::string& info_string);

	std::string user;
	std::string password;
	std::string database;
	std::string host;
	std::string port_or_socket;
};

typedef std::map<uint32_t /*index*/, std::pair<std::string /*query*/, connection_flags /*sync/async*/>> prepared_statement_map;

class sql_operation;
class prepared_statement_base;
class mysql_prepared_statement;

class mysql_connection
{
	template<typename T>
	friend class database_worker_pool;
	friend class ping_operation;
public:
	mysql_connection(mysql_connection_info& info); // 同步连接的构造函数。
	mysql_connection(producer_consumer_queue<sql_operation*>* queue, mysql_connection_info& conn_info); // 异步连接的构造函数。
	virtual ~mysql_connection();

	mysql_connection(const mysql_connection&) = delete;
	mysql_connection& operator=(const mysql_connection&) = delete;

	virtual uint32_t open();
	void close();

	bool prepare_statements();

	bool execute(const char* sql);
	bool execute(prepared_statement_base* stmt);

	result_set* query(const char* sql);
	prepared_result_set* query(prepared_statement_base* stmt);

	bool query(const char* sql, MYSQL_RES** preult, MYSQL_FIELD** fields, uint64_t* row_count, uint32_t* field_count);
	bool query(prepared_statement_base* stmt, MYSQL_RES** result, uint64_t* row_count, uint32_t* field_count);

	void begin_transaction();
	void rollback_transaction();
	void commit_transaction();
	int execute_transaction(std::shared_ptr<transaction_base> transaction);

	void ping();

	uint32_t get_last_error();
protected:
	// 试图获得锁定。 如果另一个线程获取了锁，则调用父级将尝试另一个连接
	bool lock_if_ready();

	// 由父数据库池调用。 将让其他线程访问此连接
	void unlock();

	MYSQL* get_handle() { return mysql_; }
	mysql_prepared_statement* get_prepared_statement(uint32_t index);
	void prepared_statement(uint32_t index, const char* sql, connection_flags flag);

	virtual void do_prepare_statements() = 0;

	std::vector<std::unique_ptr<mysql_prepared_statement>> stmts_; // 准备好的声明存储
	prepared_statement_map queries_; // 查询存储
	bool reconnection_; // 我们重新连接了吗？
	bool prepare_error_; // 在准备陈述时是否有任何错误？
private:
	bool handle_mysql_errno(uint32_t error_code, uint8_t attempts = 5);

	producer_consumer_queue<sql_operation*>* queue_; // 队列与其他异步连接共享。
	std::unique_ptr<database_worker> worker_; // 核心工作者任务。
	MYSQL* mysql_;
	mysql_connection_info& connection_info_;
	connection_flags connection_flags_;
	std::mutex mutex_;
};

#endif
