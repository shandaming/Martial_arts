/*
 * Copyright (C) 2018
 */

#ifndef NET_EPOLL_H
#define NET_EPOLL_H

#include <map>
#include <vector>

class channel;
class event_loop;

class epoll
{
public:
	typedef std::vector<channel*> channel_list;

	epoll(event_loop* loop);
	~epoll();

	void poll(int timeout_ms, channel_list* active_channels);
	void update_channel(channel* channel);
	void remove_channel(channel* channel);

	bool has_channel(channel* channel) const;
	void assert_in_loop_thread() const;
private:
	typedef std::map<int, channel*> channel_map;
	channel_map channels_;

	static constexpr int event_list_size = 16;
	static const char* operation_to_string(int op);

	void fill_active_channels(int num_events,
			channel_list* active_channels) const;
	void update(int operation, channel* channel);

	typedef std::vector<struct epoll_event> event_list;

	event_loop* owner_loop_;
	int epollfd_;
	event_list events_;
};

#endif
