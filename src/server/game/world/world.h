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

#include "database_env_fwd.h"
#include "locked_queue.h"
#include "query_callback_processor.h"
#include "timer.h"

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
	CONFIG_MAX_OVERSPEED_PING,
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
struct realm;

extern struct realm realm;

typedef std::unordered_map<uint32_t, world_session*> session_map;

class world
{
public:
	static world* instance();

	void add_session(world_session* session);
	void add_instance_socket(std::weak_ptr<world_socket> sock, uint64_t connect_to_key);

	void set_int_config(world_int_configs index, uint32_t value)
	{
		if(index < INT_CONFIG_VALUE_COUNT)
			int_configs_[index] = value;
	}

	uint32_t get_int_configs(world_int_configs index)
	{
		return index < INT_CONFIG_VALUE_COUNT ? int_configs_[index] : 0;
	}

	bool is_closed() const;
	void set_closed(bool val);

	account_types get_player_security_limit() const { return allowed_security_level_; }
private:
	world();
	~world();

	bool is_closed_;
	account_types allowed_security_level_;

	typedef std::unordered_map<uint32_t, time_t> disconnect_map;

	session_map sessions_;
	disconnect_map disconnects_;

	uint32_t int_configs_[INT_CONFIG_VALUE_COUNT];

	locked_queue<world_session*> add_sess_queue_;

	locked_queue<std::pair<std::weak_ptr<world_socket>, uint64_t>> link_socket_queue_;
};

#define WORLD world::instance()

#endif
