#ifndef GUI_EVENT_DISPATCHER_H
#define GUI_EVENT_DISPATCHER_H

#include "gui/core/event/handler.h"
#include "hotkey/hotkey_command.hpp"
#include "utils/general.hpp"

#include <SDL2/SDL_events.h>

#include <cassert>
#include <functional>
#include <list>
#include <map>
#include <type_traits>

struct point;
class widget;
struct message;

template<typename... T>
using dispatcher_callback = std::function<void(widget&, const ui_event, bool&, bool&, T...)>;

using signal = dispatcher_callback<>;

using signal_mouse = dispatcher_callback<const point&>;

using signal_keyboard = dispatcher_callback<const SDL_Keycode, const SDL_Keymod, const std::string&>;

using signal_touch_motion = dispatcher_callback<const point&, const point&>;
using signal_touch_gesture = dispatcher_callback<const point&, float, float, uint8_t>;

using signal_notification = dispatcher_callback<void*>;

using signal_message = dispatcher_callback<const message&>;

using signal_raw_event = dispatcher_callback<const SDL_Event&>;

using signal_text_input = dispatcher_callback<const std::string&, int32_t, int32_t>;

using hotkey_function = std::function<void(widget& dispatcher, hotkey::HOTKEY_COMMAND id)>;

class dispatcher
{
	friend struct dispatcher_implementation;
public:
	dispatcher();
	virtual ~dispatcher();

	void connect();

	virtual bool is_at(const point& coordinate) const = 0;

    enum event_queue_type
    {
		pre = 1,
		child = 2,
		post = 4
	};

	bool has_event(const ui_event event, const event_queue_type event_type);

	bool fire(const ui_event event, widget& target);
	bool fire(const ui_event event, widget& target, const point& coordinate);
    bool fire(const ui_event event, widget& target, const SDL_Keycode key,
        const SDL_Keymod modifier, const std::string& unicode);
	bool fire(const ui_event event, widget& target, const point& pos, const point& distance);
    bool fire(const ui_event event, widget& target, const point& center, float dTheta,
              float dDist, uint8_t numFingers);
	bool fire(const ui_event event, widget& target, void*);
	bool fire(const ui_event event, widget& target, const message& msg);
	bool fire(const ui_event event, widget& target, const SDL_Event& sdlevent);
	bool fire(const ui_event event, widget& target, const std::string& text, int32_t start, int32_t len);

    enum queue_position
    {
		front_pre_child,
		back_pre_child,
		front_child,
		back_child,
		front_post_child,
		back_post_child
	};

	template<ui_event E, typename F>
	void connect_signal(const F& func, const queue_position position = back_child)
	{
		get_signal_queue<get_event_category(E)>().connect_signal(E, position, func);
	}

	template<ui_event E, typename F>
	void disconnect_signal(const F& func, const queue_position position = back_child)
	{
		get_signal_queue<get_event_category(E)>().disconnect_signal(E, position, func);
	}

    enum class mouse_behavior
    {
		all,
		hit,
		none
	};

	void capture_mouse()
	{
        capture_mouse(this);
	}

	void release_mouse()
	{
        release_mouse(this);
	}

	void set_mouse_behavior(const mouse_behavior mouse_behavior)
	{
		mouse_behavior_ = mouse_behavior;
	}

	mouse_behavior get_mouse_behavior() const
	{
		return mouse_behavior_;
	}

	void set_want_keyboard_input(const bool want_keyboard_input)
	{
		want_keyboard_input_ = want_keyboard_input;
	}

	bool get_want_keyboard_input() const
	{
		return want_keyboard_input_;
	}

	void register_hotkey(const hotkey::HOTKEY_COMMAND id, const hotkey_function& function);
	bool execute_hotkey(const hotkey::HOTKEY_COMMAND id);
private:
	template<class T>
	struct signal_type
	{
		signal_type() = default;

		std::list<T> pre_child;
		std::list<T> child;
		std::list<T> post_child;

		bool empty(const dispatcher::event_queue_type queue_type) const
		{
            if((queue_type & dispatcher::pre) && !pre_child.empty())
				return false;

            if((queue_type & dispatcher::child) && !child.empty())
				return false;

            if((queue_type & dispatcher::post) && !post_child.empty())
				return false;

			return true;
		}
	};

	template<class T>
	struct signal_queue
	{
		signal_queue() = default;

		signal_queue(const signal_queue&) = delete;
		signal_queue& operator=(const signal_queue&) = delete;

		using callback = T;
		std::map<ui_event, signal_type<T>> queue;

		void connect_signal(const ui_event event, const queue_position position, const T& signal)
		{
            switch(position)
            {
			case front_pre_child:
				queue[event].pre_child.push_front(signal);
				break;
			case back_pre_child:
				queue[event].pre_child.push_back(signal);
				break;

			case front_child:
				queue[event].child.push_front(signal);
				break;
			case back_child:
				queue[event].child.push_back(signal);
				break;

			case front_post_child:
				queue[event].post_child.push_front(signal);
				break;
			case back_post_child:
				queue[event].post_child.push_back(signal);
				break;
			}
		}

		void disconnect_signal(const ui_event event, const queue_position position, const T& signal)
		{
			const auto predicate = [&signal](const T& element) { return signal.target_type() == element.target_type(); };

            switch(position)
            {
			case front_pre_child:
				[[fallthrough]];
			case back_pre_child:
				queue[event].pre_child.remove_if(predicate);
				break;

			case front_child:
				[[fallthrough]];
			case back_child:
				queue[event].child.remove_if(predicate);
				break;

			case front_post_child:
				[[fallthrough]];
			case back_post_child:
				queue[event].post_child.remove_if(predicate);
				break;
			}
		}
	};

	mouse_behavior mouse_behavior_;

	bool want_keyboard_input_;

	signal_queue<signal> signal_queue_;
	signal_queue<signal_mouse> signal_mouse_queue_;
	signal_queue<signal_keyboard> signal_keyboard_queue_;
	signal_queue<signal_touch_motion> signal_touch_motion_queue_;
	signal_queue<signal_touch_gesture> signal_touch_gesture_queue_;
	signal_queue<signal_notification> signal_notification_queue_;
	signal_queue<signal_message> signal_message_queue_;
	signal_queue<signal_raw_event> signal_raw_event_queue_;
	signal_queue<signal_text_input> signal_text_input_queue_;

	bool connected_;

	std::map<hotkey::HOTKEY_COMMAND, hotkey_function> hotkeys_;

	template<event_category cat>
	auto& get_signal_queue()
	{
        if constexpr(cat == event_category::general)
			return signal_queue_;
        else if constexpr(cat == event_category::mouse)
			return signal_mouse_queue_;
        else if constexpr(cat == event_category::keyboard)
			return signal_keyboard_queue_;
        else if constexpr(cat == event_category::touch_motion)
			return signal_touch_motion_queue_;
        else if constexpr(cat == event_category::touch_gesture)
			return signal_touch_gesture_queue_;
        else if constexpr(cat == event_category::notification)
			return signal_notification_queue_;
        else if constexpr(cat == event_category::message)
			return signal_message_queue_;
        else if constexpr(cat == event_category::raw_event)
			return signal_raw_event_queue_;
        else if constexpr(cat == event_category::text_input)
			return signal_text_input_queue_;
        else
			static_assert(utils::dependent_false_v<decltype(cat)>, "No matching signal queue for category");
	}
};

void connect_signal_pre_key_press(dispatcher& dispatcher, const signal_keyboard& signal);

void connect_signal_mouse_left_click(dispatcher& dispatcher, const signal& signal);
void disconnect_signal_mouse_left_click(dispatcher& dispatcher, const signal& signal);

void connect_signal_mouse_left_release(dispatcher& dispatcher, const signal& signal);
void disconnect_signal_mouse_left_release(dispatcher& dispatcher, const signal& signal);

void connect_signal_mouse_left_double_click(dispatcher& dispatcher, const signal& signal);

void connect_signal_notify_modified(dispatcher& dispatcher, const signal_notification& signal);

void connect_signal_on_draw(dispatcher& dispatcher, const signal& signal);

#endif
