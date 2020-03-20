/*
 * Copyright (C) 2018
 */

#ifndef NET_EPOLL_H
#define NET_EPOLL_H

#include <map>

class channel;
class event_loop;

class epoll
{
public:
	typedef std::vector<channel*> channel_list;

	epoll(event_loop* loop);
	~epoll();

	/// Polls the I/O events.
	/// Must be called in the loop thread.
	void poll(int timeout_ms, channel_list* active_channels);

	/// Changes the interested I/O events.
	/// Must be called in the loop thread.
	void update_channel(channel* channel);

	/// Remove the channel, when it destructs.
	/// Must be called in the loop thread.
	void remove_channel(channel* channel);

	bool has_channel(channel* channel) const;

	void assert_in_loop_thread() const;
private:
	typedef std::map<int, channel*> channel_map;
	channel_map channels_;

	event_loop* owner_loop_;

	static const int event_list_size = 16;
	static const char* operation_to_string(int op);

	void fill_active_channels(int num_events,
			channel_list* active_channels) const;
	void update(int operation, channel* channel);

	typedef std::vector<struct epoll_event> event_list;

	int epollfd_;
	event_list events_;
};

#endif
