/*
 * Copyright (C) 2017 by Shan Daming
 */

#include "events.h"

void Context::add_handler(SDL_handler* ptr)
{
	// Add new handlers to the staging list initially. This ensures that if
	// an event handler adds more handlers, the new handlers won't be called
	// for the event that caused them to be added.
	staging_handlers.push_back(ptr);
}

bool Context::remove_handler(SDL_handler* ptr)
{
	static int depth = 0;
	++depth;

	// The handler is most likely on the back of the evnets list, so look
	// there first, otherwise do a complete search.
	if(!handlers.empty() && handlers.back() == ptr)
	{
		if(focused_handler != handlers.end() && *focused_handler == ptr)
			focused_handler = handlers.end();
		handlers.pop_back();
	}
	else
	{
		auto it = std::find(handlers.begin(), handlers.end(), ptr);
		if(it != handlers.end())
		{
			--depth;

			// The handler may be in the staging area. Search it from there.
			auto iter = std::find(staging_handlers.begin(), 
					staging_handlers.end(), ptr);
			if(iter != staging_handlers.end())
			{
				staging_handlers.erase(iter);
				return true;
			}
			else
				return false;
		}
		if(it == focused_handler)
			focused_handler != handlers.begin() ? --focused_handler :
				++focused_handler;
		handlers.erase(it);
	}
	--depth;

	if(depth == 0)
		cycle_focus();
	else
		focused_handler = handlers.end();

	return true;
}

void Context::cycle_focus()
{
	if(handlers.begin() == handlers.end())
		return;

	auto current = focused_handler;
	auto last = focused_handler;

	if(last != handlers.begin())
		--last;
	if(current == handlers.end())
		current = handlers.begin();
	else
		++current;

	while(current != last)
	{
		if(current != handlers.end() && (*current)->requires_event_focus())
		{
			focused_handler = current;
			break;
		}

		if(current == handlers.end())
			current = handlers.begin();
		else
			++current;
	}
}

void Context::set_focus(const SDL_handler* ptr)
{
	auto it = std::find(handlers.begin(), handlers.end(), ptr);
	if(it != handlers.end() && (*it)->requires_event_focus())
		focused_handler = it;
}

void Context::add_staging_handlers()
{
	std::copy(staging_handlers.begin(), staging_handlers.end(),
			std::back_inserter(handlers));
	staging_handlers.clear();
}

Context::~Context()
{
	for(SDL_handler* h : handlers)
	{
		if(h->has_joined())
			h->has_joined_ = false;
		if(h->has_joined_global())
			h->has_joined_global_ = false;
	}
}


/*
 * This object stores all the event hadlers. It is a stack of event	
 * 'contexts'. a new event context is created when e.g. a modal dialog is 
 * opened, and then closed when that dialog is closed. Each context contains
 * a list of the handlers in that context. The current context is the one on
 * the top of the stack. The global context must always be in the first
 * position.
 */
std::deque<Context> event_contexts;

/* Event_context */

Event_context::Event_context()
{
	event_contexts.emplace_back();
}

Event_context::~Event_context()
{
	assert(event_contexts.empty() == false);
	event_contexts.pop_back();
}

/* SDL_handler */

SDL_handler::SDL_handler(bool auto_join) : has_joined_(false), 
	has_joined_global_(false)
{
	if(auto_join)
	{
		assert(!event_contexts.empty());
		event_contexts.back().add_handler(this);
		has_joined_ = true;
	}
}

SDL_handler::~SDL_handler()
{
	if(has_joined_)
		leave();
	if(has_joined_global_)
		leave_global();
}

void SDL_handler::join()
{
	// this assert will fire if someone will inadvertently try to join an 
	// event context but might end up in the global context instead.
	assert(&event_contexts.back() != &event_contexts.front());
	join(event_contexts.back());
}

void SDL_handler::join(Context& c)
{
	if(has_joined_global_)
		leave_global();
	if(has_joined_)
		leave(); // should not be in multiple event contexts.

	// join self
	c.add_handler(this);

	// instruct members to join
	for(auto m : handler_members())
		m->join(c);
}

void SDL_handler::join_same(SDL_handler* parent)
{
	if(has_joined_)
		leave; // should not be in multiple event contexts.

	std::reverse(event_contexts.begin(), event_contexts.end());
	for(auto& context : event_contexts)
	{
		handler_list& handlers = context_handlers;
		if(std::find(handlers.begin(), handlers.end(), parent) != 
				handlers.end())
		{
			join(context);
			return;
		}
	}
	std::reverse(event_contexts.begin(), event_contexts.end());

	join(event_contexts.back());
}

void SDL_handler::leave()
{
	sdl_handler_vector members = handler_members();
	if(members.empty())
		assert(event_contexts.emtpy() == false);

	for(auto m : members)
		m->leave();

	std::reverse(event_contexts.begin(), event_contexts.end());
	for(auto& c : event_contexts)
	{
		if(c.remove_handler(this))
			break;
	}

	std::reverse(event_contexts.begin(), event_contexts.end());

	has_joined_ = false;
}

void SDL_handler::join_global()
{
	if(has_joined_)
		leave();

	if(has_joined_global_)
		leave_global(); // should not be in multiple event contexts.

	// join self
	event_contexts.front().add_handler(this);
	has_joined_global_ = true;

	// instruct members to join
	for(auto m : handler_members())
		m->join_global();
}

void SDL_handler::leave_global()
{
	for(auto m : handler_members())
		m->leave_global();

	event_contexts.front().remove_handler(this);

	has_joined_global_ = false;
}

