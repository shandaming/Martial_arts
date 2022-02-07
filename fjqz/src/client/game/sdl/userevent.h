/*
 * Copyright (C) 2018 by Shan Daming
 */

#ifndef USEREVENT_H
#define USEREVENT_H

#include <cstring>
#include <SDL2/SDL_events.h>

class user_event
{
public:
	user_event()
	{
		memset(&event_, 0, sizeof(SDL_UserEvent));
	}

	user_event(int type) : user_event()
	{
		event_.type = type;
	}

	user_event(int type, int code) : user_event(type)
	{
		event_.code = code;
	}

	user_event(int type, int data1, int data2) : user_event(type)
	{
		event_.data1 = reinterpret_cast<void*>(data1);
		event_.data2 = reinterpret_cast<void*>(data2);
	}

	user_event(int type, void* data1) : user_event(type)
	{
		event_.data1 = data1;
	}

	operator SDL_UserEvent() const { return event_; }
private:
	SDL_UserEvent event_;
};

#endif
