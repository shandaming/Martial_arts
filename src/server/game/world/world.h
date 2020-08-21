/*
 * Copyright (C) 2020
 */

#ifndef _WORLD_H
#define _WORLD_H

#include <cstdint>
#include <atomic>
#include <map>
#include <unordered_map>
#include <vector>
#include <list>
#include <memory>

#include "timer.h"
#include "database_env_fwd.h"
#include "query_callback_processor.h"
#include "locked_queue.h"

enum world_bool_configs
{
	BOOL_CONFIG_VALUE_COUNT
};

enum world_float_configs
{
	FLOAT_CONFIG_VALUE_COUNT
};

enum world_int_configs
{
	CONFIG_COMPRESSION = 0,
	INT_CONFIG_VALUE_COUNT
};

enum world_int64_configs
{
	INT64_CONFIG_VALUE_COUNT
};

class player;
class world_packet;
class world_session;
class world_socket;

typedef std::unordered_map<uint32_t, world_session*> session_map;

class world
{
public:
	static world* instance();

	void set_int_config(world_int_configs index, uint32_t value)
	{
		if(index < INT_CONFIG_VALUE_COUNT)
			int_configs_[index] = value;
	}

	uint32_t get_int_configs(world_int_configs index)
	{
		return index < INT_CONFIG_VALUE_COUNT ? int_configs_[index] : 0;
	}
private:
	world();
	~world();

	typedef std::unordered_map<uint32_t, time_t> disconnect_map;

	session_map sessions_;
	disconnect_map disconnects_;

	uint32_t int_configs_[INT_CONFIG_VALUE_COUNT];
};

#define WORLD world::instance()

#endif
