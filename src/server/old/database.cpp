/*
 * Copyright (C) 2018
 */

#include "database.h"

namespace db
{
	std::vector<std::vector<std::string>> query_data(MYSQL* mysql, 
			const std::string& sql)
	{
		std::vector<std::vector<std::string>> result;
		MYSQL_RES* res = nullptr;

		if(mysql_query(mysql, sql.c_str()))
			return result;

		// 获取查询结果集
		if(!res = mysql_store_result(mysql))
			return result;

		int num_fields = mysql_num_fields(res);	// 获取查询中字段个数
		std::vector<std::string> tmp;

		// 遍历每行
		for(MYSQL_ROW r; r = mysql_fetch_row(res); )
		{
			for(int i = 0; i < num_fields; ++i)
				tmp.emplace_back(r[i]);
			result.emplace_back(tmp);
		}
		if(res)
			mmysql_free_result(res);
		return result;
	}

	bool insert_data(MYSQL* mysql, const std::string& sql)
	{
		if(mysql_query(mysql, sql.c_str()))
			return false;
		return true;
	}

	bool update_data(MYSQL* mysql, const std::string& sql)
	{
		if(mysql_query(mysql, sql.c_str()))
			return false;
		return true;
	}

	bool delete_data(MYSQL* mysql, const std::string& sql)
	{
		if(mysql_query(mysql, sql.c_str()))
			return false;
		return true;
	}
}
