/*
 * Copyright (C) 2018
 */

#ifndef CONNECTION_POOL_H
#define CONNECTION_POOL_H

#include <deque>
#include <set>
#include <mutex>
#include <string>

class Mysql_connection_factory
{
	public:
		Mysql_connection_factory(const std::string& host, const int port,
				const std::string& database, const std::string& user,
				const std::string& passwrod);

		MYSQL* create()
		{
			MYSQL* sql;
			mysql_init(sql);
			if(mysql_real_connect(mysql, host.c_str(), user.c_str(), 
						password.c_str(), database.c_str(), port, nullptr, 
						0))
				return nullptr;
			return mysql;
		}
	private:
		std::string host_;
		int port_;
		std::string database_;
		std::string user_;
		std::string password_;
};

class Connection_pool
{
	public:
		Connection_pool(size_t pool_size, Mysql_connection_factory* factory) : pool_size_(size), factory_(factory)
		{
			while(pool_.size() < pool_size_)
				pool_.emplace_back(factory->create());
		}

		~Connection_pool()
		{
			for(int i = 0; i < pool_.size(); ++i)
			{
				mysql_close(pool_.front());
				pool_.pop_front();
			}
			for(auto & c : borrowed_)
				mysql_close(c);
			if(factory_)
				delete factory_;
		}

		MYSQL* borrow()
		{
			std::scoped_lock lock(mutex_);

			// 如果当前连接都用完了，再创建2倍连接,注意：不要超过数据库最大
			// 连接数
			if(pool_.size() == 0)
			{
				for(int i = pool_size_; i < pool_size_ * 2; ++i)
				{
					pool_.emplace_back(factory->create());
				}
			}
			MYSQL* sql = pool_.front();
			pool_.pop_front();
			borrowed_.emplace(sql);
			return sql;
		}

		void unborrow(MYSQL* conn)
		{
			std::scoped_lock lock(mutex_);
			pool_.emplace_back(conn);

			borrowed_.erase(conn);

			// 空闲连接已经超过创建的连接数
			if(pool_.size() > pool_size_)
			{
				// 销毁额外创建的连接， 使连接回归正常创建的数
				for(int i = 0; i < pool_.size() - pool_size_; ++i)
				{
					mysql_close(pool_.front());
					pool_.pop_front();
				}
			}
		}
	private:
		size_t pool_size_;
		Mysql_connection_factory* factory_;
		std::deque<MYSQL*> pool_;
		std::set<MYSQL*> borrowed_;
		std::mutex mutex_;
};

#endif
