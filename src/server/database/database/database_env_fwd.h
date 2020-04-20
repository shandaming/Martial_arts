/*
 * Copyright (C) 2019
 */

#ifndef DB_DATABASE_ENV_FWD_H
#define DB_DATABASE_ENV_FWD_H

#include <memory>
#include <future>

class result_set;
typedef std::shared_ptr<result_set> query_result;
typedef std::future<query_result> query_result_future;
typedef std::promise<query_result> query_result_promise;

class prepared_result_set;
typedef std::shared_ptr<prepared_result_set> prepared_query_result;
typedef std::future<prepared_query_result> prepared_query_result_future;
typedef std::promise<prepared_query_result> prepared_query_result_promise;

class sql_query_holder_base;
typedef std::future<sql_query_holder_base*> query_result_holder_future;
typedef std::promise<sql_query_holder_base*> query_result_holder_promise;

#endif
