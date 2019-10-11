/*
 * Copyright (C) 2018
 */

#ifndef EVENT_H
#define EVENT_H

namespace event
{
	enum Socket_event
	{
		SOCKET,
		BIND,
		LISTEN,
		ACCEPT,
		CONNECT,

		EPOLL_ADD,
		EPOLL_READ,
		EPOLL_WRITE
	};

	class Event_handler;
	typedef vector<Event_handler*> Handler_list;

	struct Context
	{
		Context() : handlers() {}
		~Context();

		void add_handler(Event_handler* ptr);
		bool remove_handler(Event_handler* ptr);

		Handler_list handlers;
	};

	class Event_handler
	{
			friend struct Context;
		public:
			Event_handler();

			Event_handler(const Eevent_handler&) = delete;
			Event_handler& operator=(const Event_handler&) = delete;

			virtual ~Event_handler();

			virtual handle_event() = 0;
			virtual void process_event() {}

			virtual bool write_handler(Event_handler* handler) = 0;
			virtual bool read_handler(Event_handler* handler) = 0;
		protected:
			virtual Handler_list handler_members() 
			{ 
				return Handler_list(); 
			}
	};

	void run_event_loop();
}

#endif
