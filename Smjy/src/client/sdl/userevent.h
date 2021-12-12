/*
 * Copyright (C) 2018 by Shan Daming
 */

#ifndef USEREVENT_H
#define USEREVENT_H

#include <cstring>
#include <SDL2/SDL_events.h>

namespace sdl
{
	class User_event
	{
		public:
			User_event()
			{
				memset(&event_, 0, sizeof(SDL_UserEvent));
			}

			User_event(int type) : User_event()
			{
				event_.type = type;
			}

			User_event(int type, int code) : User_event(type)
			{
				event_.code = code;
			}

			User_event(int type, int data1, int data2) : User_event(type)
			{
				event_.data1 = reinterpret_cast<void*>(data1);
				event_.data2 = reinterpret_cast<void*>(data2);
			}

			User_event(int type, void* data1) : User_event(type)
			{
				event_.data1 = data1;
			}

			operator SDL_UserEvent() { return event_; }
		private:
			SDL_UserEvent event_;
	};
}

#endif
