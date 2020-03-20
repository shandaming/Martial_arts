/*
 * Copyright (C) 2018
 */

#ifndef CHANNEL_H
#define CHANNEL_H

#include <functional>
#include <memory>

class event_loop;

class channel
{
public:
	typedef std::function<void()> event_callback;
	typedef std::function<void(Timestamp)> read_event_callback;

	channel(event_loop* loop, int sockfd);
	~channel();

	// ---------------Setting callback---------------

	void set_read_callback(read_event_callback&& cb)
	{ 
		read_callback_ = std::move(cb); 
	}

	void set_write_callback(event_callback&& cb)
	{
		write_callback_ = std::move(cb); 
	}

	void set_close_callback(event_callback&& cb)
	{
		close_callback_ = std::move(cb); 
	}

	void set_error_callback(event_callback&& cb)
	{
		error_callback_ = std::move(cb); 
	}

	int get_descriptor() const { return sockfd_; }
	int events() const { return events_; }
	void set_revents(int revt) { revents_ = revt; } // used by pollers
  
	bool is_none_event() const { return events_ == none_event_; }

	// 读写事件注册
	
	void enable_read() 
	{
		events_ |= read_event_; 
		update(); 
	}

	void disable_read() 
	{
		events_ &= ~read_event_; 
		update(); 
	}

	void enable_write() 
	{
		events_ |= write_event_; 
		update(); 
	}

	void disable_write() 
	{
		events_ &= ~write_event_; 
		update(); 
	}

	void disable_all() 
	{
		events_ = none_event_; 
		update(); 
	}

	bool is_write() const { return events_ & write_event_; }
	bool is_read() const { return events_ & read_event_; }

	// for Poller
	enum state : int8_t
	{
		NEW = -1,
		ADDED = 1,
		DELETED
	};

	int get_state() const { return state_; }
	void set_state(state state) { state_ = state; }

	void handle_event();

	// for debug
	std::string revents_to_string() const;
	std::string events_to_string() const;

	event_loop* owner_loop() { return loop_; }
	void remove();
private:
	static std::string events_to_string(int fd, int ev);

	void update();
			
	void handle_event_with_guard();

	static const int none_event_;
	static const int read_event_;
	static const int write_event_;

	event_loop* loop_;

	int sockfd_;
	int events_;
	int revents_; // it's the received event types of epoll or poll
	state state_; // used by Poller.

	bool event_handling_;
	bool added_to_loop_;

	read_event_callback read_callback_;
	event_callback write_callback_;
	event_callback close_callback_;
	event_callback error_callback_;
};

#endif
