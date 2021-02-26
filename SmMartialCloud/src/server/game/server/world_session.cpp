/*
 * Copyright (C) 2020
 */

#include "world_session.h"

world_session::world_session(uint32_t id, std::string&& name, uint32_t account_id, std::shared_ptr<world_socket> sock, account_types sec, uint8_t expansion, time_t mute_time, st::string& os, locale_constant locale, uint32_t recruiter, bool is_a_recruiter) :
	mute_time_(mute_time),
	time_out_time_(0),
	anti_dos(this),
	guid_low(ui64lit(0)),
	player_(NULL),
	security_(sec),
	account_id_(id),
	account_name_(std::move(name)),
	battlenet_account_id_(battlenet_account_id),
	account_expansion_(expansion),
	expansion_(st::min<uint8_t>(expansion, WORLD->get_int_config(CONFIG_EXPANSION))),
	os_(os),
	battlenet_request_token_(0),
	warden_(NULL),
	logout_time_(0),
	in_queue_(false),
	player_logout_(false),
	player_recently_logout_(false),
	player_save_(false),
	session_dbc_locale(WORLD->get_available_dbc_locale(locale)),
	session_db_locale_index_(locale),
	latency_(0),
	client_time_delay_(0),
	tutorials_changed_(TUTORIALS_FLAG_NONE),
	filter_addon_messages_(false),
	recruiter_id_(recruiter),
	rbac_data_(NULL),
	expire_time_(60000),
	force_exit_(false),
	current_banker_guid(),
	battle_pet_mgr(std::make_unique<battle_pet_mgr>(this)),
	collection_mgr(std::make_unique<collection_mgr>(this))
{
	memset(tutorials_, 0, sizeof(tutorials_));
	if(sock)
	{
		address_ = sock->get_remote_ip_address().to_string();
		reset_timeout_time();
		login_database.pexecute("UPDATE account SET online = 1 WHERE id = %u;", get_account_id());
	}

	socket_[CONNECTION_TYPE_REALM] = sock;
	instance_connect_key.raw = ui64lit(0);
}

std::string world_session::get_player_info() const
{
	std::stringstream os;
	os << "[Player: ";
	if(player_loading_.is_empty())
		os << "Logging in: " << player_loading_.to_string() << ", ";
	else if(player_)
		os << player_->get_name() << " " << player->get_guid().to_string() << ", ";

	os << "Account: " << get_account_id() << "]";

	return os.str();
}

void world_session::queue_packet(world_packet* new_packet)
{
	recv_queue_.add(new_packet);
}

void world_session::reset_timeout_time()
{
	timeout_time = int32_t(WORLD->get_int_config(CONFIG_SOCKET_TIMEOUTTIME));
}
