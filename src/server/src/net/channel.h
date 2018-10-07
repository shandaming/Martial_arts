/*
 * Copyright (C) 2018
 */

#ifndef CHANNEL_H
#define CHANNEL_H

#include <functional>
#include <memory>
#include "common/timestamp.h"

namespace net
{
class Event_loop;

	class Channel
	{
		public:
			typedef std::function<void()> Event_callback;
			typedef std::function<void(Timestamp)> Read_event_callback;

			Channel(Event_loop* loop, int fd);
			~Channel();

			// ---------------Setting callback---------------

			void set_read_callback(Read_event_callback&& cb)
			{ 
				read_callback_ = std::move(cb); 
			}

			void set_write_callback(Event_callback&& cb)
			{
				write_callback_ = std::move(cb); 
			}

			void set_close_callback(Event_callback&& cb)
			{
				close_callback_ = std::move(cb); 
			}

			void set_error_callback(Event_callback&& cb)
			{
				error_callback_ = std::move(cb); 
			}

			// 将此通道绑定到shared_ptr管理的所有者对象，防止在handle_event
			// 中销毁所有者对象。
			void tie(const std::shared_ptr<void>&);

			int fd() const { return fd_; }
			int events() const { return events_; }
			void set_revents(int revt) { revents_ = revt; } // used by pollers
  
			bool is_none_event() const { return events_ == kNone_event; }

			void enable_read() { events_ |= kRead_event; update(); }
			void disable_read() { events_ &= ~kRead_event; update(); }
			void enable_write() { events_ |= kWrite_event; update(); }
			void disable_write() { events_ &= ~kWrite_event; update(); }
			void disable_all() { events_ = kNone_event; update(); }
			bool is_write() const { return events_ & kWrite_event; }
			bool is_read() const { return events_ & kRead_event; }

			// for Poller
			int index() { return index_; }
			void set_index(int idx) { index_ = idx; }

			void handle_event(Timestamp&& receive_time);

			// for debug
			std::string revents_to_string() const;
			std::string events_to_string() const;

			void do_not_log_hup() { log_hup_ = false; }

			Event_loop* owner_loop() { return loop_; }
			void remove();

		private:
			static std::string events_to_string(int fd, int ev);

			void update();
			
			void handle_event_with_guard(Timestamp&& receive_time);

			static const int kNone_event;
			static const int kRead_event;
			static const int kWrite_event;

			Event_loop* loop_;
			const int  fd_;
			int events_;
			int revents_; // it's the received event types of epoll or poll
			int index_; // used by Poller.
			bool log_hup_;

			std::weak_ptr<void> tie_;
			bool tied_;
			bool event_handling_;
			bool added_to_loop_;

			Read_event_callback read_callback_;
			Event_callback write_callback_;
			Event_callback close_callback_;
			Event_callback error_callback_;
	};
}

#endif
