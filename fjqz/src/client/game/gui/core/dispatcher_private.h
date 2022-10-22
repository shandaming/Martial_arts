#ifndef GUI_EVENT_DISPATCHER_PRIVATE_H
#define GUI_EVENT_DISPATCHER_PRIVATE_H

#include "gui/core/event/dispatcher.h"

#include "gui/widgets/widget.hpp"
#include "utils/ranges.hpp"

#include <SDL2/SDL_events.h>

#include <cassert>

struct dispatcher_implementation
{
	template<event_category C>
	static auto& event_signal(dispatcher& dispatcher, const ui_event event)
	{
		return dispatcher.get_signal_queue<C>().queue[event];
	}

	static bool has_handler(dispatcher& dispatcher, const dispatcher::event_queue_type queue_type, ui_event event)
	{
        const auto queue_check = [&](auto& queue_set)
        {
			return !queue_set.queue[event].empty(queue_type);
		};

		// We can't just use get_signal_queue since there's no way to know the event at compile time.
        switch(get_event_category(event))
        {
		case event_category::general:
			return queue_check(dispatcher.signal_queue_);
		case event_category::mouse:
			return queue_check(dispatcher.signal_mouse_queue_);
		case event_category::keyboard:
			return queue_check(dispatcher.signal_keyboard_queue_);
		case event_category::touch_motion:
			return queue_check(dispatcher.signal_touch_motion_queue_);
		case event_category::touch_gesture:
			return queue_check(dispatcher.signal_touch_gesture_queue_);
		case event_category::notification:
			return queue_check(dispatcher.signal_notification_queue_);;
		case event_category::message:
			return queue_check(dispatcher.signal_message_queue_);
		case event_category::raw_event:
			return queue_check(dispatcher.signal_raw_event_queue_);
		case event_category::text_input:
			return queue_check(dispatcher.signal_text_input_queue_);
		default:
			throw std::invalid_argument("Event is not categorized");
		}
	}
};

namespace implementation
{
template<event_category C>
std::vector<std::pair<widget*, ui_event>>
build_event_chain(const ui_event event, widget* dispatcher, widget* w)
{
    assert(dispatcher && w);

	std::vector<std::pair<widget*, ui_event>> result;

    while(true)
    {
        if(w->has_event(event, dispatcher::event_queue_type(dispatcher::pre | dispatcher::post)))
			result.emplace_back(w, event);

        if(w == dispatcher)
			break;

		w = w->parent();
		assert(w);
	}

	return result;
}

template<>
std::vector<std::pair<widget*, ui_event>>
build_event_chain<event_category::notification>(const ui_event event, widget* dispatcher, widget* w)
{
    assert(dispatcher && w);

	assert(!w->has_event(event, dispatcher::event_queue_type(dispatcher::pre | dispatcher::post)));

	return {};
}

template<>
std::vector<std::pair<widget*, ui_event>>
build_event_chain<event_category::message>(const ui_event event, widget* dispatcher, widget* w)
{
    assert(dispatcher && w && dispatcher == w);

	std::vector<std::pair<widget*, ui_event>> result;

	/* We only should add the parents of the widget to the chain. */
    while((w = w->parent()))
    {
		assert(w);

        if(w->has_event(event, dispatcher::event_queue_type(dispatcher::pre | dispatcher::post)))
			result.emplace(result.begin(), w, event);
	}

	return result;
}

template<event_category C, typename... F>
bool fire_event(const ui_event event,
	const std::vector<std::pair<widget*, ui_event>>& event_chain,
	widget* dispatcher,
	widget* w,
	F&&... params)
{
	bool handled = false;
	bool halt = false;

	/***** ***** ***** Pre ***** ***** *****/
    for(const auto& [chain_target, chain_event] : utils::reversed_view(event_chain))
    {
		const auto& signal = dispatcher_implementation::event_signal<C>(*chain_target, chain_event);

        for(const auto& pre_func : signal.pre_child)
        {
			pre_func(*dispatcher, chain_event, handled, halt, std::forward<F>(params)...);

            if(halt)
            {
				assert(handled);
				break;
			}
		}

        if(handled)
			return true;
	}

	/***** ***** ***** Child ***** ***** *****/
    if(w->has_event(event, dispatcher::child))
    {
		const auto& signal = dispatcher_implementation::event_signal<C>(*w, event);

        for(const auto& func : signal.child)
        {
			func(*dispatcher, event, handled, halt, std::forward<F>(params)...);

            if(halt)
            {
				assert(handled);
				break;
			}
		}

        if(handled)
			return true;
	}

	/***** ***** ***** Post ***** ***** *****/
    for(const auto& [chain_target, chain_event] : event_chain)
    {
		const auto& signal = dispatcher_implementation::event_signal<C>(*chain_target, chain_event);

        for(const auto& post_func : signal.post_child)
        {
			post_func(*dispatcher, chain_event, handled, halt, std::forward<F>(params)...);

            if(halt)
            {
				assert(handled);
				break;
			}
		}

        if(handled)
			return true;
	}

	/**** ***** ***** Unhandled ***** ***** *****/
	assert(handled == false);
	return false;
}

} // namespace implementation

template<event_category C, typename... F>
bool fire_event(const ui_event event, dispatcher* d, widget* w, F&&... params)
{
    assert(d && w);

	widget* dispatcher_w = dynamic_cast<widget*>(d);

	std::vector<std::pair<widget*, ui_event>> event_chain =
		implementation::build_event_chain<C>(event, dispatcher_w, w);

	return implementation::fire_event<C>(event, event_chain, dispatcher_w, w, std::forward<F>(params)...);
}

template<ui_event click,
	ui_event double_click,
	bool (event_executor::*wants_double_click)() const,
	typename... F>
bool fire_event_double_click(dispatcher* dsp, widget* wgt, F&&... params)
{
    assert(dsp && wgt);

	std::vector<std::pair<widget*, ui_event>> event_chain;
	widget* w = wgt;
	widget* d = dynamic_cast<widget*>(dsp);

    while(w != d)
    {
		w = w->parent();
		assert(w);

        if(std::invoke(wants_double_click, w))
        {
            if(w->has_event(double_click, dispatcher::event_queue_type(dispatcher::pre | dispatcher::post)))
				event_chain.emplace_back(w, double_click);
        }
        else
        {
            if(w->has_event(click, dispatcher::event_queue_type(dispatcher::pre | dispatcher::post)))
				event_chain.emplace_back(w, click);
		}
	}

    if(std::invoke(wants_double_click, wgt))
    {
		constexpr auto C = get_event_category(double_click);
		return implementation::fire_event<C>(double_click, event_chain, d, wgt, std::forward<F>(params)...);
    }
    else
    {
		constexpr auto C = get_event_category(click);
		return implementation::fire_event<C>(click, event_chain, d, wgt, std::forward<F>(params)...);
	}
}

#endif
