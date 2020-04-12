/*
 * Copyright (C) 2019
 */

#include <mysql/mysql.h>
#include <mysql/mysqld_error.h>

#include <cstring>

#include "mysql_connection.h"
#include "mysql_prepared_statement.h"
#include "prepared_statement.h"
#include "string_utils.h"
#include "log.h"
#include "timer.h"
#include "errors.h"

mysql_connection_info::mysql_connection_info(const std::string& info_string){
	std::vector<std::string> tokens = split(info_string, ';');
	if(tokens.size() != 5)
		return;

	uint8_t i = 0;
	host.assign(tokens[i++]);
	port_or_socket.assign(tokens[i++]);
	user.assign(tokens[i++]);
	password.assign(tokens[i++]);
	database.assign(tokens[i++]);
}

mysql_connection::mysql_connection(mysql_connection_info& info) : reconnection_(false), prepare_error_(false), queue_(nullptr), mysql_(nullptr), connection_info_(info), connection_flags_(CONNECTION_SYNCH) {}

mysql_connection::mysql_connection(producer_consumer_queue<sql_operation*>* queue, mysql_connection_info& conn_info) : 
	reconnection_(false), prepare_error_(false), queue_(queue), mysql_(NULL), connection_info_(conn_info), connection_flags_(CONNECTION_ASYNC)
{
	worker_ = std::make_unique<database_worker>(queue_, this);
}

mysql_connection::~mysql_connection() { close(); }

uint32_t mysql_connection::open()
{
	MYSQL* mysql;
	mysql = mysql_init(nullptr);
	if(!mysql)
	{
		LOG_ERROR("sql.sql", "Could not initialize MySql connection to database %s", connection_info_.database.c_str());
		return CR_UNKNOWN_ERROR;
	}

	mysql_options(mysql, MYSQL_SET_CHARSET_NAME, "utf8");
	int port;
	const char* unix_socket;
	if(connection_info_.host == ".")
	{
		unsigned int opt = MYSQL_PROTOCOL_SOCKET;
		mysql_options(mysql, MYSQL_OPT_PROTOCOL, (const char*)&opt);
		connection_info_.host = "localhost";
		port = 0;
		unix_socket = connection_info_.port_or_socket.c_str();
	}
	else
	{
		port = atoi(connection_info_.port_or_socket.c_str());
		unix_socket = nullptr;
	}

	mysql_ = mysql_real_connect(mysql, connection_info_.host.c_str(), connection_info_.user.c_str(), connection_info_.password.c_str(), connection_info_.database.c_str(), port, unix_socket, 0);
	if(mysql_)
	{
		if(!reconnection_)
		{
			LOG_INFO("sql.sql", "MySQL client library: %s", mysql_get_client_info());
			LOG_INFO("sql.sql", "MySQL server ver: %s", mysql_get_server_info(mysql_));
		}

		LOG_INFO("sql.sql", "Connected to MySQL database at %s", connection_info_.host.c_str());
		mysql_autocommit(mysql_, 1);

		//将连接属性设置为UTF8以正确处理不同的语言环境
		// server configs  -  core以UTF8发送数据，因此MySQL也必须期待UTF8
		mysql_set_character_set(mysql_, "utf8");
		return 0;
	}
	else
	{
		LOG_ERROR("sql.sql", "Could not connect to MySQL database at %s : %s", connection_info_.host.c_str(), mysql_error(mysql));
		mysql_close(mysql);
		return mysql_errno(mysql);
	}
}

void mysql_connection::close()
{
	// 在清除之前停止工作线程
	worker_.reset();
	stmts_.clear();

	if(mysql_)
	{
		mysql_close(mysql_);
		mysql_ = nullptr;
	}
}

bool mysql_connection::prepare_statements()
{
	do_prepare_statements();
	return !prepare_error_;
}

bool mysql_connection::execute(const char* sql)
{
	if(!mysql_)
		return false;

	uint32_t start_ms = get_ms_time();
	if(mysql_query(mysql_, sql))
	{
		uint32_t errno_code = mysql_errno(mysql_);

		LOG_INFO("sql.sql", "SQL:%s", sql);
		LOG_ERROR("sql.sql", "[ %u ] %s", errno_code, mysql_error(mysql_));

		if(handle_mysql_errno(errno_code)) // 如果返回true，则成功处理错误（即重新连接）
			return execute(sql); //再试一次
		return false;
	}
	else
		LOG_DEBUG("sql.sql", "[%u ms] SQL:%s", get_ms_time_diff(start_ms, get_ms_time()), sql);
	return true;
}

bool mysql_connection::execute(prepared_statement_base* stmt)
{
	if(!mysql_)
		return false;
	uint32_t index = stmt->index_;
	mysql_prepared_statement* prepared_stmt = get_prepared_statement(index);
	ASSERT(prepared_stmt);	// 如果准备失败，服务器端错误或查询错误，则只能为null
	prepared_stmt->prepared_stmt_ = stmt; // 交叉引用它们以进行调试输出
	stmt->stmt_ = prepared_stmt; //

	stmt->bind_parameters();
	MYSQL_STMT* mysql_stmt = prepared_stmt->get_stmt();
	MYSQL_BIND* mysql_bind = prepared_stmt->get_bind();

	uint64_t start_ms = get_ms_time();

	if(mysql_stmt_bind_param(mysql_stmt, mysql_bind))
	{
		uint32_t errno_code = mysql_errno(mysql_);
		LOG_ERROR("sql.sql", "SQL(p): %s\n [ERROR]: [%d] %s", prepared_stmt->get_query_string(queries_[index].first).c_str(), errno_code, mysql_stmt_error(mysql_stmt));
		if(handle_mysql_errno(errno_code)) // 如果返回true，则成功处理错误（即重新连接）
			return execute(stmt); 
		prepared_stmt->clear_parameters();
		return false;
	}
	if(mysql_stmt_execute(mysql_stmt))
	{
		uint32_t errno_code = mysql_errno(mysql_);
		LOG_ERROR("sql.sql", "SQL(p): %s\n [ERROR]: [%d] %s", prepared_stmt->get_query_string(queries_[index].first).c_str(), errno_code, mysql_stmt_error(mysql_stmt));
		if(handle_mysql_errno(errno_code)) // If it returns true, an error was handled successfully (i.e. reconnection)
			return execute(stmt); 
		prepared_stmt->clear_parameters();
		return false;
	}

	LOG_DEBUG("sql.sql", "[%d ms] SQL(p): %s", get_ms_time_diff(start_ms, get_ms_time()), prepared_stmt->get_query_string(queries_[index].first).c_str());

	prepared_stmt->clear_parameters();
	return true;
}

bool mysql_connection::query(prepared_statement_base* stmt, MYSQL_RES** result, uint64_t* row_count, uint32_t* field_count)
{
	if(!mysql_)
		return false;

	uint32_t index = stmt->index_;
	mysql_prepared_statement* prepared_stmt = get_prepared_statement(index);
	ASSERT(prepared_stmt); // 如果准备失败，服务器端错误或查询错误，则只能为null
	prepared_stmt->prepared_stmt_ = stmt; // 交叉引用它们以进行调试输出
	stmt->stmt_ = prepared_stmt; //

	stmt->bind_parameters();
	MYSQL_STMT* mysql_stmt = prepared_stmt->get_stmt();
	MYSQL_BIND* mysql_bind = prepared_stmt->get_bind();
	uint64_t start_ms = get_ms_time();
	if(mysql_stmt_bind_param(mysql_stmt, mysql_bind))
	{
		uint32_t errno_code = mysql_errno(mysql_);
		LOG_ERROR("sql.sql", "SQL(p): %s\n [ERROR]: [%d] %s", prepared_stmt->get_query_string(queries_[index].first).c_str(), errno_code, mysql_stmt_error(mysql_stmt));
		if(handle_mysql_errno(errno_code)) // 如果返回true，则成功处理错误（即重新连接）
			return query(stmt, result, row_count, field_count); 
		prepared_stmt->clear_parameters();
		return false;
	}
	if(mysql_stmt_execute(mysql_stmt))
	{
		uint32_t errno_code = mysql_errno(mysql_);
		LOG_ERROR("sql.sql", "SQL(p): %s\n [ERROR]: [%d] %s", prepared_stmt->get_query_string(queries_[index].first).c_str(), errno_code, mysql_stmt_error(mysql_stmt));
		if(handle_mysql_errno(errno_code)) // 如果返回true，则成功处理错误（即重新连接）
			return query(stmt, result, row_count, field_count); 
		prepared_stmt->clear_parameters();
		return false;
	}

	LOG_DEBUG("sql.sql", "[%d ms] SQL(p): %s", get_ms_time_diff(start_ms, get_ms_time()), prepared_stmt->get_query_string(queries_[index].first).c_str());

	prepared_stmt->clear_parameters();
	*result = mysql_stmt_result_metadata(mysql_stmt);
	*row_count = mysql_stmt_num_rows(mysql_stmt);
	*field_count = mysql_stmt_field_count(mysql_stmt);

	return true;
}

result_set* mysql_connection::query(const char* sql)
{
	if(!sql)
		return NULL;
	MYSQL_RES* result = 0;
	MYSQL_FIELD* fields = 0;
	uint64_t row_count = 0;
	uint32_t field_count = 0;

	if(!query(sql, &result, &fields, &row_count, &field_count))
		return NULL;
	return new result_set(result, fields, row_count, field_count);
}

bool mysql_connection::query(const char* sql, MYSQL_RES** result, MYSQL_FIELD** fields, uint64_t* row_count, uint32_t* field_count)
{
	if(!mysql_)
		return false;
	uint32_t start_ms = get_ms_time();
	if(mysql_query(mysql_, sql))
	{
		uint32_t errno_code = mysql_errno(mysql_);

		LOG_INFO("sql.sql", "SQL: %s", sql);
		LOG_ERROR("sql.sql", "[%d] %s", errno_code, mysql_error(mysql_));

		if(handle_mysql_errno(errno_code)) // 如果返回true，则成功处理错误（即重新连接）
			return query(sql, result, fields, row_count, field_count);
		return false;
	}
	else
		LOG_DEBUG("sql.sql", "[%d ms] SQL: %s", get_ms_time_diff(start_ms, get_ms_time()), sql);

	*result = mysql_store_result(mysql_);
	*row_count = mysql_affected_rows(mysql_);
	*field_count = mysql_field_count(mysql_);

	if(!*result)
		return false;
	if(!*row_count)
	{
		mysql_free_result(*result);
		return false;
	}
	*fields = mysql_fetch_fields(*result);
	return true;
}

void mysql_connection::begin_transaction()
{
	execute("START TRANSACTION");
}

void mysql_connection::rollback_transaction()
{
	execute("ROLLBACK");
}

void mysql_connection::commit_transaction()
{
	execute("COMMIT");
}

int mysql_connection::execute_transaction(std::shared_ptr<transaction_base>& transaction)
{
	const std::vector<sql_element_data> queries = transaction->queries_;
	if(queries.empty())
		return -1;

	begin_transaction();

	for(auto it = queries.begin(); it != queries.end(); ++it)
	{
		const sql_element_data& data = *it;
		switch(it->type)
		{
			case SQL_ELEMENT_PREPARED:
				{
					prepared_statement_base* stmt = data.element.stmt;
					ASSERT(stmt);
					if(!execute(stmt))
					{
						LOG_WARN("sql.sql", "Transaction aborted. %lu queries not executed.", queries.size());

						int error_code = get_last_error();
						rollback_transaction();
						return error_code;
					}
				}
				break;
			case SQL_ELEMENT_RAW:
				{
					const char* sql = data.element.query;
					ASSERT(sql);
					if(!execute(sql))
					{
						LOG_WARN("sql.sql", "Transaction abored. %lu queries not executed.", queries.size());

						int error_code = get_last_error();
						rollback_transaction();
						return error_code;
					}
				}
				break;
			default:
				break;
		}
	}
	// 我们可能会在某些查询期间遇到错误，并且根据我们可能希望重新启动事务的错误类型。 因此，为防止数据丢失，我们只会在完成所有操作后进行清理。 这是在调用函数DatabaseWorkerPool <T> :: DirectCommitTransaction和TransactionTask :: Execute时完成的，而不是在迭代每个元素时完成的。
	
	commit_transaction();
	return 0;
}

void mysql_connection::ping()
{
	mysql_ping(mysql_);
}

uint32_t mysql_connection::get_last_error()
{
	return mysql_errno(mysql_);
}

bool mysql_connection::lock_if_ready()
{
	return mutex_.try_lock();
}

void mysql_connection::unlock()
{
	mutex_.unlock();
}

mysql_prepared_statement* mysql_connection::get_prepared_statement(uint32_t index)
{
	ASSERT(index < stmts_.size());
	mysql_prepared_statement* ret = stmts_[index].get();
	if(!ret)
		LOG_ERROR("sql.sql", "Could not fetch prepared statement %u on database '%s', connectin type: %s", index, connection_info_.database.c_str(), (connection_flags_ & CONNECTION_ASYNC) ? "asynchronous" : "synchronous");
	return ret;
}

void mysql_connection::prepared_statement(uint32_t index, const char* sql, connection_flags flags)
{
	queries_.insert(prepared_statement_map::value_type(index, std::make_pair(sql, flags)));

	// 检查是否应在此连接上准备指定的查询，即不要在同步连接上准备异步语句以节省不使用的内存。
	if(!(connection_flags_ & flags))
	{
		stmts_[index].reset();
		return;
	}

	MYSQL_STMT* stmt = mysql_stmt_init(mysql_);
	if(!stmt)
	{
		LOG_ERROR("sql.sql", "In mysql_stmt_init() id: %u, sql: \"%s\"", index, sql);
		LOG_ERROR("sql.sql", "%s", mysql_error(mysql_));
		prepare_error_ = true;
	}
	else
	{
		if(mysql_stmt_prepare(stmt, sql, static_cast<unsigned long>(strlen(sql))))
		{
			LOG_ERROR("sql.sql", "In mysql_stmt_prepare() id:%u, sql \"%s\"", index, sql);
			LOG_ERROR("sql.sql", "%s", mysql_stmt_error(stmt));
			mysql_stmt_close(stmt);
			prepare_error_ = true;
		}
		else
		{
			stmts_[index] = std::make_unique<mysql_prepared_statement>(stmt);
		}
	}
}

prepared_result_set* mysql_connection::query(prepared_statement_base* stmt)
{
	MYSQL_RES* result = 0;
	uint64_t row_count = 0;
	uint32_t field_count = 0;

	if(!query(stmt, &result, &row_count, &field_count))
		return NULL;
	if(mysql_more_results(mysql_))
		mysql_next_result(mysql_);
	return new prepared_result_set(stmt->stmt_->get_stmt(), result, row_count, field_count);
}

bool mysql_connection::handle_mysql_errno(uint32_t error_code, uint8_t attempts)
{
	switch(error_code)
	{
		case CR_SERVER_GONE_ERROR:
		case CR_SERVER_LOST:
		case CR_INVALID_CONN_HANDLE:
		case CR_SERVER_LOST_EXTENDED:
			{
				if(mysql_)
				{
					LOG_ERROR("sql.sql", "Lost the connection to the MySQL server!");

					mysql_close(get_handle());
					mysql_ = 0;
				}
			}
		case CR_CONN_HOST_ERROR:
			{
				LOG_INFO("sql.sql", "Attempting to reconnect to the MySQL server ...");

				reconnection_ = true;
				const uint32_t err = open();
				if(!err)
				{
					// 除非你想跳过加载所有准备好的语句，否则不要删除'this'指针...
					if(!this->prepare_statements())
					{
						LOG_FATAL("sql.sql", "Could not re-prepare statements!");

						std::this_thread::sleep_for(std::chrono::seconds(10));
						abort();
					}

					LOG_INFO("sql.sql", "Successfully reconnected to %s @%s:%s (%s).", connection_info_.database.c_str(), connection_info_.host.c_str(), connection_info_.port_or_socket.c_str(), (connection_flags_ & CONNECTION_ASYNC) ? "asynchronous" : "synchronous");

					reconnection_ = false;
					return true;
				}
				if( --attempts == 0)
				{
					// 当mysql服务器无法访问一段时间时关闭服务器
					LOG_FATAL("sql.sql", "Failed to reconnect to the MySQL server, terminating the server to prevent data corruption!");

					// 我们也可以通过使用std :: raise（SIGTERM）来启动关闭
					std::this_thread::sleep_for(std::chrono::seconds(10));
					abort();
				}
				else
				{
					// 2006年我们可能会尝试重新连接。 为了防止疯狂的递归呼叫，请在这里睡觉。
					std::this_thread::sleep_for(std::chrono::seconds(3));
					return handle_mysql_errno(err, attempts); // 打电话给自己（递归）
				}
			}
		case ER_LOCK_DEADLOCK:
			return false; //  在TransactionTask :: Execute和DatabaseWorkerPool <T> :: DirectCommitTransaction中实现
			// 查询相关错误 - 跳过查询
		case ER_WRONG_VALUE_COUNT:
		case ER_DUP_ENTRY:
			return false;
			// 过时的表或数据库结构 - 终止核心
		case ER_BAD_FIELD_ERROR:
		case ER_NO_SUCH_TABLE:
			LOG_ERROR("sql.sql", "Your database structure is not up to date. Please make sure you've executed all queries in the sql/updates folders.");

			std::this_thread::sleep_for(std::chrono::seconds(10));
			abort();
			return false;
		case ER_PARSE_ERROR:
			LOG_ERROR("sql.sql", "Error while parsing SQL. Core fix requires.");

			std::this_thread::sleep_for(std::chrono::seconds(10));
			abort();
			return false;
		default:
			LOG_ERROR("sql.sql", "Unhandled MySQL errno %d. Unexpected behaviour possible.", error_code);
			return false;
	}
}
