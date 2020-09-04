/*
 * Copyright (C) 2020
 */

#include "world_session.h"

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
