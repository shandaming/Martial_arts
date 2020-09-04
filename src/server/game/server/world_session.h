/*
 * Copyright (C) 2020
 */

#ifndef _WORLD_SESSION_H
#define _WORLD_SESSION_H

#include "common.h"

class player;

class world_session
{
public:
	world_session(uint32_t id, std::string&& name, uint32_t account_id, std::shared_ptr<world_socket> sock, account_types sec, uint8_t expansion, time_t mute_time, std::string& os, locale_constant locale, uint32_t recruiter, bool is_a_recruiter);
	~world_session();

	uint32_t get_account_id() const { return account_id_; }
	std::string get_player_info() const;
private:
	world_session(const world_session&) = delete;
	world_session& operator=(const world_session&) = delete;

	player* palyer_;
	std::shared_ptr<world_socket> socket_[MAX_CONNECTION_TYPES];
	std::string address_;
	uint32_t account_id_;
};

#endif
