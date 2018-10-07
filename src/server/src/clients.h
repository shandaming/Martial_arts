/*
 * Copyright (C) 2018
 */

#ifndef CLIENTS_H
#define CLIENTS_H

typedef std::map<int, Player*> Map_player;

class Clients
{
	public:
		void add_player(int fd, Player* player) 
		{
			players_[fd] = player; 
		}

		Map_player get_players() const { return players_; }
	private:
		Map_player players_;
};

#endif
