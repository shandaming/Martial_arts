/*
 * Copyright (C) 2018
 */

#include <cstring>
#include "mysql_pool.h"

namespace db
{
	Query::Query() : dataset_(nullptr), field_(nullptr), row_(nullptr),
		field_count_(0), row_count_(0) {}

	Query::Query(MYSQL_RES* dataset) { set_dataset(dataset); }

	Query::Query(const Query& query) : dataset_(query.dataset_),
		field_(query.field_), row_(query.row_), 
		field_count_(query.field_count_),
		row_count_(query.row_count_) {}

	Query& Query::operator=(const Query& query)
	{
		dataset_ = query.dataset_;
		field_ = query.field_;
		row_ = query.row_;
		field_count_ = query.field_count_;
		row_count_ = query.row_count_;
		return *this;
	}

	void set_dataset(MYSQL_RES* dataset)
	{
		field_ = row_ = nullptr;
		field_count_ = row_count_ = 0;
		dataset_ = dataset;
		get_dataset_info();
	}

	void Query::move_first()
	{
		if(dataset_)
			mysql_data_seek(dataset_, 0);
	}

	void Query::move_last()
	{
		if(dataset_)
			mysql_data_seek(dataset_, row_count_ - 1);
	}

	void Query::next_row()
	{
		if(dataset_)
			row_ = mysql_fetch_row(dataset_);
	}

	int Query::get_filed_index(const std::string& field)
	{
		if(!dataset_ || field.emtpy() || !field_)
			return -1;

		for(int i = 0; i < field_count_; ++i)
		{
			if(strcasecmp(field, filed_[i].name) == 0)
				return i;
		}
		return -1;
	}

	std::string Query::get_field_value(const std::string& field)
	{
		if(!dataset_ || field.empty() || !field_)
			return std::string();

		int field_index = get_field_index(filed);
		if(field_index >= 0 && field_index < field_count_)
		{
			if(row_[field_index] == nullptr)
				return std::string();

			return row_[field_index];
		}
		return std::string();
	}

	float Query::get_filed_num(const std::string& field)
	{
		std::string res = get_filed_value(field);
		if(!res.empty())
			return static_cast<float>(atof(res));
		return res;
	}

	std::string Query::get_filed_value(int field_index)
	{
		if(dataset_ && field_index >= 0 && field_index < fiel_count_)
			return row_[field_index];
		return std::string();
	}

	void get_dataset_info()
	{
		if(dataset_)
		{
			move_first();
			row_ = mysql_fetch_row(dataset_);
			row_count_ = mysql_num_rows(dataset_);

			field_count_ = mysql_num_fields(dataset_);
			field_ = mysql_fetch_fields(dataset_);
		}
	}

	void Query::finalize()
	{
		if(dataset_)
		{
			mysql_free_result(dataset_);
			dataset_ = nullptr;
		}
		field_count = row_count_ = 0;

		field_ = row = nullptr;
	}

	Database::Database() : mysql_(nullptr), mutil_statements_(true)
	{
		reset_connect_info();
		effect_row_ = 0;
	}

	Database::Database(const std::string& host, 
					const std::string& db_name, 
					const std::string& user_name, 
					const std::string& password,
					const int port,
					int client_opt)
	{
		assert(open(host, db_name, user_name, password, port, client_opt));
	}

	Database::~Database() { close(); }

	void Database::set_conn_info(const std::string& host, 
			const std::string& db_name, 
			const std::string& user_name, 
			const std::string& password,
			const int port)
	{
		host_ = host;
		db_name_ = db_name;
		user_name_ = user_name;
		password_ = password;
		port_ = port;
	}

	bool Database::open(const std::string& host, const std::string& db_name,
			const std::string& user_name, const std::string& password,
			const int port, int client_opt)
	{
		close();
		mysql_library_init(0, nullptr, nullptr);
		mysql_ = mysql_init(nullptr);
		if(!mysql_)
			return false;

		int timeout = 60;
		mysql_options(mysql_, MYSQL_OPT_CONNECT_TIMEOUT, static_cast<void*>(&timeout));
		mysql_options(mysql_, MYSQL_OPT_READ_TIMEOUT, static_cast<void*>(&timeout));
		mysql_options(mysql_, MYSQL_OPT_WRITE_TIMEOUT, static_cast<void*>(&timeout));

		if(mysql_real_connect(mysql_, host, user_name, password, db_name,
					port, nullptr, CLIENT_BASIC_FLAGS & ~CLIENT_NO_SCHEMA &
					~CLIENT_IGNORE_SIGPIPE) == nullptr)
			return false;

		if(!set_character_set("utf8"))
			return false;
		mutil_statements_ = client_opt & CLIENT_MULTI_STATEMENTS;
		set_conn_info(host, db_name, user_name, password, port);
		return true;
	}

	void Database::close()
	{
		if(mysql_)
		{
			mysql_close(mysql_);
			mysql_ = nullptr;
			mysql_library_end();
		}
	}

	bool Database::execute_non_query(const std::string& sql)
	{
		effect_row_ = 0;
		if(!mysql_)
		{
			if(!reconnect())
				return false;
		}

		if(mysql_query(mysql_, sql.c_str()))
		{
			if(mysql_errno(mysql_) == 2006 || mysql_errno(mysql_) == 2013)
			{
				if(!reconnect())
					return false;
				if(mysql_query(mysql_, sql.c_str()))
					return false;
			}
			else
				return false;
		}

		if(mutil_statements_)
		{
			do
			{
				effect_row_ += mysql_affected_rows(mysql_);
			}
			while(!mysql_next_result(mysql_));
		}
		else
			effect_rpw_ = mysql_affected_rows(mysql_);
		return true;
	}

	Query Database::execute_query(const std::string& sql)
	{
		if(!mysql_)
		{
			if(!reconnect())
				return nullptr;
		}

		MYSQL_RES* res = nullptr;
		if(mysql_real_query(mysql_, sql.c_str(), sql.len()))
		{
			if(mysql_errno(mysql_) == 2006 || mysql_errno(mysql_) == 2013)
			{
				if(!reconnect())
					return nullptr;
				if(mysql_real_query(mysql_, sql.c_str(), sql.size()))
					return nullptr;
			}
			else
				return nullptr;
		}

		do
		{
			MYSQL_RES* tmp = mysql_store_result(mysql_);
			if(tmp == nullptr)
				break;
			res = tmp;	// 只处理第一个数据集
		}
		while(!mysql_next_result(mysql_));
		return res;
	}

	bool Database::execute_query(const std::string& sql, Query& query)
	{
		query = ececute_query(sql);
		if(query.null())
			return false;
		return true;
	}

	bool Database::begin_transaction()
	{
		if(mysql_ == nullptr)
		{
			if(!reconnect())
				return false;

			std::string str = "START TRANSACTION";
			if(mysql_real_query(mysql_, str.c_str(), str.size()) < 0)
				return true;
		}
		return false;
	}

	bool Database::commit()
	{
		if(mysql_ == nullptr)
		{
			if(!reconnect())
				return false;
		}
		
		std::string str = "COMMIT";
		if(!mysql_real_query(mysql_, str.c_str(), str.size()))
			return true;
		return false;
	}

	bool Database::rollback()
	{
		std::string str = "ROLLBACK";
		if(!mysql_real_query(mysql_, str.c_str(), str.size()))
			return true;
		return false;
	}

	bool Database::reconnect()
	{
		if(open(host_, db_name_, user_name_, password_, port_, CLIENT_BASIC_FLAGS & ~CLIENT_NO_SCHEMA & ~CLIENT_IGNORE_SIGPIPE))
			return true;
		return false;
	}

	void reset_connect_info()
	{
		user_name_.clear();
		password_.clear();
		db_name_.clear();
		host_.clear();
	}

	bool Database::set_character_set(const std::string& encoding)
	{
		int ret = 0;
		if(mysql_)
			ret = mysql_set_character_set(mysql_, encoding);
		return ret;
	}

	std::string Database::get_last_error()
	{
		std::string res;
		if(mysql_)
			ret = "errno = " + std::to_string(mysql_errno(mysql_)) +
				" err = " + std::string(mysql_error(mysql_)) + "\n";
		return ret;
	}

	Connection_factory::Connection_factory(const std::string& host, 
			const std::string& db_name, const std::string& user_name, 
			const std::string& password, const int port, int client_opt) :
		host_(host), db_name_(db_name), user_name_(user_name),
		password_(password), port_(port), client_opt_(client_opt) {}

	Database* Connection_factory::create()
	{
		Database* db = new Database(host, db_name, user_name, password,
				port, client_opt);
		if(!db)
			return nullptr;
		return db;
	}

	Connection_pool::Connection_pool() : size_(0) {}

	Connection_pool::Connection_pool(size_t size) : size_(size)
	{
		create(size_);
	}

	void Connection_pool::create(size_t size)
	{
		while(pool_.size() < size_)
			pool_.emplace_back(factory_.create());
	}

	Connection_pool::~Connection_pool()
	{
		for(auto& c : borrowed_)
		{
			if(c)
				delete c;
		}
	}

	Database* Connection_pool::borrow()
	{
		std::scoped_lock lock(mutex_);
		cv_.wait(lock, pool_.empty());

		Database* db = pool_.front();
		pool_.pop_front();

		borrowed_.emplace(db);
		return db;
	}

	void Connection_pool::unborrow(Database* db)
	{
		std::scoped_lock lock(mutex_);
		pool_.emplace_back(db);

		borrowed_.erase(db);
	}

	Query Connection_pool::execute_query(const std::string& sql)
	{
		Query query();
		Database* db = borrow();
		if(db)
		{
			query = db->execute_query(sql);
		}
		unborrow(db);
		return query;
	}

	bool Connection_pool::execute_query(const std::string& sql, Query& query)
	{
		Database* db = borrow();
		if(db)
		{
			query = db->execute_query(sql);
			unborrow(db);
			return true;
		}
		return false;
	}

	bool Connection_pool::execute_non_query(const std::string& sql)
	{
		Database* db = borrow();
		if(db)
			return db->execute_non_query(sql);
		return false;
	}
}
