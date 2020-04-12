/*
 * Copyright (C) 2019
 */

#include <algorithm>
//#include <iterator>

#include "query_callback_processor.h"

void query_callback_processor::add_query(query_callback&& query)
{
	callbacks_.emplace_back(std::move(query));
}

void query_callback_processor::process_ready_queries()
{
	if(callbacks_.empty())
	{
		return;
	}

	std::vector<query_callback> update_callbacks(std::move(callbacks_));

	update_callbacks.erase(std::remove_if(update_callbacks.begin(), update_callbacks.end(), [](query_callback&& callback)
				{
				 return callback.invoke_if_ready() == query_callback::completed;
				}), update_callbacks.end());

	callbacks_.insert(callbacks_.end(), std::make_move_iterator(update_callbacks.begin(), std::make_move_iterator(update_callbacks.end())));
}
