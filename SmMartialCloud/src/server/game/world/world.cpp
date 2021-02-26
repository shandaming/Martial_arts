/*
 * Copyright (C) 2020
 */

#include "realm.h"
#include "script_mgr.h"
#include "world.h"

world::world() : is_closed_(false), allowed_security_level_(SEC_PLAYER)
{}

world::~world()
{}

void world::add_session(world_session* session)
{
	add_sess_queue_.add(session);
}

void world::add_instance_socket(std::weak_ptr<world_socket> sock, uint64_t connect_to_key)
{
	link_socket_queue_.add(std::make_pair(sock, connect_to_key));
}

bool world::is_closed() const { return is_closed_; }
void world::set_closed(bool val)
{
	is_closed_ = val;
	SCRIPT_MGR->on_open_state_change(!val);
}

world* world::instance()
{
	static world instance;
	return &instance;
}

struct realm realm;
