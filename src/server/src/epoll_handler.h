/*
 * Copyright (C) 2018
 */

#ifndef EPOLL_HANDLER_H
#define EPOLL_HANDLER_H

namespace event
{
	class Epoll_handler : public Event_handler
	{
		public:
			Epoll_handler(int max = 64);
			~Epoll_handler();

			bool run_event_loop();

			void connect(Dispatcher* dispatcher);
			void disconnect(Dispatcher* dispatcher);
		private:
			int max_;
			int epollfd_;
			epoll_event* events_;
			std::vector<Dispatcher*> dispatchers_;
	};
}

#endif
