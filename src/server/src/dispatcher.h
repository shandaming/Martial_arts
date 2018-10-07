/*
 * Copyright (C) 2018
 */

#ifndef DISPATCHER_H
#define DISPATCHER_H

namespace event
{
	class Dispatcher
	{
		public:
			Dispatcher();

			Dispatcher(const Dispatcher&) = delete;
			Dispatcher& operator=(const Dispatcher&) = delete;

			~Dispatcher();

			bool fire(const Socket_event event, Player& target);
		private:
	};
}

#endif
