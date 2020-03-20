/*
 * Copyright (C) 2018
 */

#ifndef NET_POLLER_H
#define NET_POLLER_H

#include <map>

#include "channel.h"

class Event_loop;

class Poller
{
public:
	typedef std::vector<Channel*> Channel_list;

	Poller(Event_loop* loop);
	~Poller();

	/// Polls the I/O events.
	/// Must be called in the loop thread.
	void poll(int timeoutMs, Channel_list* activeChannels);

	/// Changes the interested I/O events.
	/// Must be called in the loop thread.
	void update_channel(Channel* channel);

	/// Remove the channel, when it destructs.
	/// Must be called in the loop thread.
	void remove_channel(Channel* channel);

	bool has_channel(Channel* channel) const;

	static Poller* new_default_poller(Event_loop* loop);

	void assert_in_loop_thread() const;
private:
	typedef std::map<int, Channel*> Channel_map;
	Channel_map channels_;

	Event_loop* owner_loop_;

	static const int kInit_event_list_size = 16;

	static const char* operation_to_string(int op);

	void fill_active_channels(int num_events,
			Channel_list* active_channels) const;
	void update(int operation, Channel* channel);

	typedef std::vector<struct epoll_event> Event_list;

	int epollfd_;
	Event_list events_;
};

#endif
