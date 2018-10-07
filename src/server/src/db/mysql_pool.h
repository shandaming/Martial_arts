/*
 * Copyright (C) 2018
 */

#ifndef MYSQL_POOL_H
#define MYSQL_POOL_H

#include <string>
#include <dequeue>
#include <set>
#include <mutex>
#include <condition_variable>
#include <mysql/mysql.h>

namespace db
{
	class Query
	{
			friend class Database;
		public:
			Query();
			Query(MYSQL_RES* dataset);
			Query& operator=(const Query& query);

			~Query() { finalize(); }

			// 移动到数据集的首行
			void move_fire();

			// 移动到数据集的末行
			void move_last();

			void next_row();

			// 是否数据集的行尾
			bool is_eof() { return row_ == nullptr; }

			// 释放指针资源
			void finalize();

			bool is_primary_key(const int index);

			bool null() { return dataset_ == nullptr; }

			/***************** setter ********************/

			void set_dataset(MYSQL_RES* dataset);

			/****************** getter **********************/
			int get_row_count() const { return row_count_; }

			int get_field_count() const { return filed_count_; }

			MYSQL_FIELD* get_field() const { return field_; }

			int get_field_value_len(const int index);

			// 获取字段的值
			std::string get_field_value(const std::string& field);

			float get_field_num(const std::string& field);

			// 获取字段的值
			std::string get_field_value(int field_index);
			
		private:
			// 取子段所在的位置
			int get_field_index(const std::string& field);

			// 获取数据集的信息
			void get_dataset_info();

			MYSQL_RES* dataset_;

			MYSQL_FIELD* field_;
			int field_count_;

			MYSQL_ROW row_;
			int row_count_;
	};

	class Database
	{
		public:
			Database();
			Database(const std::string& host, 
					const std::string& db_name, 
					const std::string& user_name, 
					const std::string& password,
					const int port,
					int client_opt = CLIENT_MULTI_STATEMENTS);
			~Database();

			bool open(const std::string& host, 
					const std::string& db_name,
					const std::string& user_name, 
					const std::string& password,
					const int port = 3306, 
					int client_opt = CLIENT_MULTI_STATEMENTS);

			void close();

			// 执行无返回结果集的语句
			bool execute_non_query(const std::string& sql);

			// 执行有返回结果集的语句
			Query execute_query(const std::string& sql);
			bool execute_query(const std::string& sql, Query& query);

			// 刷新调整缓存
			bool refresh() 
			{
				return !mysql_refresh(mysql_, REFRESH_HOSTS | 
						REFRESH_TABLES);
			}

			bool ping() { return !mysql_ping(mysql_); }

			// 开始事物
			bool begin_transaction();

			// 提交事物
			bool commit();

			// 回滚事物
			bool rollback();

			/************************* setter **************************/

			void set_conn_info(const std::string& host, 
					const std::string& db_name, 
					const std::string& user_name, 
					const std::string& password,
					const int port);

			// 设置字符集编码
			bool set_character_set(const std::string& encoding);

			// 设置是否自动提交
			bool set_autocommit(bool autocommit = false)
			{
				return mysql_autocommit(mysql_, autocommit) == 0;
			}

			// 设置多条语句是否可以一起执行
			bool set_multi_statements()
			{
				return !mysql_set_server_option(mysql_, 
						MYSQL_OPTION_MULTI_STATEMENTS_ON);
			}

			// 当数据库断开时是否自动连接
			bool set_autoconnect(bool enable = true)
			{
				return !mysql_options(mysql_, MYSQL_OPT_RECONNECT, 
						static_cast<void*>(&enable));
			}

			bool shutdown() 
			{ 
				return !mysql_shutdown(mysql_, SHUTDOWN_DEFAULT); 
			}

			bool reconnect();

			void reset_connect_info();

			/************************ getter **************************/

			// 获取最后的错误
			std::string get_last_errror();

			int get_last_error() { return mysql_errno(mysql_); }

			MYSQL* get_mysql() { return mysql_; }

			// 获取最后执行无返回结果语句影响的行数
			int get_last_effect_row() { return effect_row_; }

			// 获取自动增加的键值
			size_t get_last_row_id() { return mysql_insert_id(mysql_); }
		private:
			std::string user_name_;
			std::string password_;
			std::string host_;
			std::string db_name_;
			int port_;

			MYSQL* mysql_;

			bool mutil_statements_;
			
			long effect_row_;
	};

	class Connection_factory
	{
		public:
			Connection_factory(const std::string& host, 
					const std::string& db_name,
					const std::string& user_name, 
					const std::string& password,
					const int port = 3306, 
					int client_opt = CLIENT_MULTI_STATEMENTS);

			Database* create();
		private:
			std::string host_;
			int port_;
			std::string db_name_;
			std::string user_name_;
			std::string password_;
			int client_opt_;
	};

	class Connection_pool
	{
		public:
			Connection_pool();
			Connection_pool(size_t size);
			~Connection_pool();

			bool create(size_t size);

			// 借一个数据库连接
			Database* borrow();

			// 还一个数据库连接
			void unborrow(Database* db);

			Query execute_query(const std::string& sql);
			bool execute_query(const std::string& sql, Query& query);

			bool execute_non_query(const std::string& sql);
		private:
			size_t pool_size_;
			std::deque<Database*> pool_;
			Connection_factory factory_;

			std::set<Database*> borrowed_;
			std::mutex mutex_;
			std::condition_variable cv_;
	};
}

#endif
