/*
 * Copyright (C) 2018
 */

#include "dispatcher.h"

namespace gui
{
	namespace event
	{
		Dispatcher::Dispatcher() : mouse_behavior_(all), 
			want_keyboard_input(true),
			signal_queue_(),
			signal_mouse_queue_(),
			signal_keyboard_queue_(),
			signal_notification_queue_(),
			signal_message_queue_(),
			connected_(false),
			hotkeys_() {}

		Dispatcher::~Dispatcher()
		{
			if(connected_)
				disconnect_dispatcher(this);
		}

		void Dispatcher::connect()
		{
			assert(!connected_);
			connected_ = true;
			connect_dispatcher(this);
		}

		bool Dispatcher::has_event(const Ui_event event, 
				const Event_queue_type event_type)
		{
			//
		}

		bool Dispatcher::fire(const Ui_event event, Widget& target)
		{
			assert(is_general_event(event));
			switch(event)
			{
				case LEFT_BUTTON_DOUBLE_CLICK:
					return //
			}
		}

		bool Dispatcher::fire(const Ui_event event, Widget& target, 
				const Point& coordinate)
		{
			assert(is_mouse_event(event));
			//
		}

		bool Dispatcher::fire(const Ui_event event, Widget& target,
				const SDL_Keycode key, const SDL_Keymod modifier,
				const std::string& unicode)
		{
			assert(is_keyboard_event(event));
			//
		}

		bool Dispatcher::fire(const Ui_event event, Widget& target, 
				const SDL_Event& sdlevent)
		{
			assert(is_raw_event_event(event));
			//
		}

		bool Dispatcher::fire(const Ui_event event, Widget& target, 
				cont std::string& text, int32_t start, int32_t len)
		{
			assert(is_text_input_event(event));
			//
		}

		bool Dispatcher::fire(const Ui_event event, Widget& target,
				const Point& pos, const Point& distance)
		{
			assert(is_touch_event(event));
			//
		}

		bool Dispatcher::fire(const Ui_event event, Widget& target, void*)
		{
			assert(is_notification_event(event));
			//
		}

		// TODO: is there any reason msg isn't a const reference
		bool Dispatcher::fire(const Ui_event event, Widget& target, 
				Message& msg)
		{
			assert(is_message_event(event));
			//
		}

		void Dispatcher::register_hotkey(const hotkey::Hotkey_command id,
				const Hotkey_function& function)
		{
			hotkeys_[id] = function;
		}

		bool Dispatcher::execute_hotkey(const hotkey::Hotkey_command id)
		{
			std::map<hotkey::Hotkey_comand, Hotkey_function>::iterator it = hotkeys_.find(id);
			if(it == hotkeys_.end())
				return false;

			it->second(dynamic_cast<Widget&>(*this), id);

			/*
			 * 注意：热键事件用于返回bool以指示处理状态，但是，每个用例都会
			 * 返回true并且使代码混乱，我改变了签名以返回void，但是如果需要
			 * 在热键功能上恢复bool retval，这就是它应该被处理的地方
			 */
			return true;
		}

		void connect_signal_pre_key_press(Dispatcher& dispatcher,
				const Signal_keyboard_function& signal)
		{
			dispatcher.connect_signal<SDL_KEY_DOWN>(signal, Dispatcher::front_child);
		}

		void connect_signal_mouse_left_click(Dispatcher& dispatcher, 
				const Signal_function& signal)
		{
			dispatcher.connect_signal<LEFT_BUTTON_CLICK>(signal);
		}
		
		void disconnect_signal_mouse_left_click(Dispatcher& dispatcher,
				const Signal_function& signal)
		{
			dispatcher.disconnect_signal<LEFT_BUTTON_CLICK>(signal);
		}

		void connect_signal_mouse_left_double_click(Dispatcher& dispatcher,
				const Signal_function& signal)
		{
			dispatcher.connect_signal<LEFT_BUTTON_DOUBLE_CLICK>(signal, Dispatcher::back_post_child);
		}

		void connect_signal_notify_modified(Dispatcher& dispatcher,
				const Signal_notification_function& signal)
		{
			dispatcher.connect_signal<NOTIFY_MODIFIED>(signal);
		}

void connect_signal_on_draw(Dispatcher& dispatcher, 
		const Signal_function& signal)
{
	dispatcher.connect_signal<DRAW>(signal, Dispatcher::front_child);
}
	}
}
