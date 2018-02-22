/*
 * Copyright (C) 2017 by Shan Daming
 */

#ifndef EVENTS_H
#define EVENTS_H

#include <deque>
#include <vector>
#include <list>
#include <algorithm>
#include <cassert>
#include <SDL2/SDL.h>

class SDL_handler;
typedef std::list<SDL_handler*> handler_list;

struct Context
{
	Context() : handlers(), focused_handler(handlers.end()), 
		staging_handlers() {}
	~Context();

	void add_handler(SDL_handler* ptr);
	bool remove_handler(SDL_handler* ptr);
	void cycle_focus();
	void set_focus(const SDL_handler* ptr);
	void add_staging_handlers();

	handler_list handlers;
	handler_list::iterator focused_handler;
	std::vector<SDL_handler*> staging_handlers;
};

/*
 * any classes that derive from this class will automatically receive sdl
 * events throught the handle function for their lifetime, while the event
 * context they were created in is active.
 *
 * NOTE: an Event_context object must be initialized before a handler object
 * can be initialized, and the Event_context must be destroyed after the 
 * handler is destroyed.
 */
class SDL_handler
{
		friend class Context;
	public:
		virtual void handle_event(const SDL_Event& event) = 0;
		virtual void handle_window_event(const SDL_Event& event) = 0;
		virtual void process_event() {}
		virtual void draw() {}

		virtual void volatile_draw() {}
		virtual void volatile_undraw() {}

		virtual bool requires_event_focus(const SDL_Event* e = nullptr) 
			const { return false; }

		virtual void process_help_string(int /*mousex*/, int /*mousey*/) {}
		virtual void process_tooltip_string(int /*mousex*/, 
				int /*mousey*/) {}

		virtual void join(); // joins the current event context.
		virtual void join(Context& c); // joins the specified event context

		// joins the same event context as the parent is already associated
		// widh
		virtual void join_same(SDL_handler* parent);
		virtual void leave(); // leave the event context.

		virtual void join_global(); // join the global event context.
		virtual void leave_global(); // leave the global event context.

		virtual bool has_joined() { return has_joined_; }
		virtual bool has_joined_global() { return has_joined_global_; }
	protected:
		SDL_handler(bool auto_join = true);
		virtual ~SDL_handler();

		virtual std::vector<SDL_handler*> handler_members()
		{
			return std::vector<SDL_handler*>();
		}
	private:
		bool has_joined_;
		bool has_joined_global_;
};

typedef std::vector<SDL_handler*> sdl_handler_vector;

/*
 * Event_context objects control the handler objects that SDL events are
 * sent to. When an Event_context is created, it will become the current
 * event context. Event_context objects MUST bbe created in LIFO ordering
 * in relation to each other, and in relation to handler objects. That is,
 * all Event_context objects should be created as automatic/stack variables.
 *
 * handler objects need not be created as automatic variables (e.g. you
 * could put them in a vector) however you must guarantee that handler 
 * objects are destroyed before their context is destroyed
 */
struct Event_context
{
	Event_context();
	~Event_context();
};

#endif
