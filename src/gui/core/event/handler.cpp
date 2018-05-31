/*
 * Copyright (C) 2018
 */

#include "handler.h"
#include "events.h"

/*
 * 待办事项以下项目尚未实施
 * -工具提示有一个固定的短时间，知道出现
 * -现实工具提示，知道小部件推出。
 * -帮助信息尚未显示
 *  注意：这可能是工具提示将独立于窗口提示并现实在他们自己的窗口中，因此代码
 *  在确定之后将清除。
 */

namespace gui
{
	namespace event
	{
		// static data
		static std::unique_ptr<class Sdl_event_handler> handler = nullptr;


		// 这个单例类处理所有事件
		class Sdl_event_handler : public events::Sdl_handler
		{
				friend bool gui::is_in_dialog();
			public:
				Sdl_event_handler();
				~Sdl_event_handler();

				void handle_event(const SDL_Event& event) override;
				void handle_window_event(const SDL_Event& event) override;

				// 连接调度程序
				void connect(Dispatcher* dispatcher);
				// 断开调度程序
				void disconnect(Dispatcher* dispatcher);
				// 以Z顺序返回所有调度程序
				std::vector<Dispatcher*>& get_dispatchers() 
				{ 
					return dispatchers_; 
				}

				// 捕获鼠标焦点的调度程序
				Dispatcher* mouse_focus;
			private:
				/*
				 * 重新初始化所有调度程序状态
				 * 当应用程序被激活时，这是必须的，以确保主要鼠标状态设置
				 * 正确
				 */
				void activate();

				// 触发一个原生SDL事件
				void raw_event(const SDL_Event& event);
				// 触发一个绘图事件
				virtual void draw() override final;
				// 触发一个视频调整大小事件
				void video_resize(const Point& new_size);
				// 触发一个通常鼠标事件
				void mouse(const Ui_event event, const Point& position);
				// 触发一个鼠标松开按键事件
				void mouse_button_up(const Point& position, 
						const uint8_t button);
				// 触发一个鼠标按下事件
				void mouse_button_down(const Point& position, 
						const uint8_t button);
				// 触发一个鼠标滚轮事件
				void mouse_wheel(const Point& position, int scrollx, 
						int scrolly);
				// 获取想要接收键盘输入的调度程序
				Dispatcher* keyboard_dispatcher();
				// 触发一个触摸事件
				void touch_motion(const Point& position, 
						const Point& distance);
				// 处理一个hat运动事件
				void hat_motion(const SDL_Event& event);
				// 触发一个按键按下的事件
				void key_down(const SDL_Event& event);
				// 处理按下的热键
				bool hotkey_pressed(const hotkey::Hotkey_ptr key);
				// 触发一个按键按下事件
				void key_down(const SDL_Keycode key, 
						const SDL_Keymod modifier, 
						const std::string& unicode);
				// 触发一个文本输入事件
				void text_input(const std::string& unicode);
				// 触发一个文本编辑事件
				void text_editiong(const std::string& unicode, 
						int32_t start, int32_t len);
				// 触发没有参数的键盘事件
				void keyboard(const Ui_event event);
				// 触发一个给予window ID的CLOSE_WINDOW事件
				void close_window(const unsigned window_id);

				// 类表中的项目顺序是Z顺序，前面的项目在后，后面的项目在前
				std::vector<Dispatcher*> dispatchers_;

				// 需要确定哪个调度程序获取键盘
				// 注意：键盘事件还没有连接
				Dispatcher* keyboard_foucs_;
				
				friend void capture_keyboard(Dispatcher*);
		};

		Sdl_event_handler::Sdl_event_handler() : events::Sdl_handler(false),
			mouse_focus(nullptr), dispatchers(), keyboard_focus_(nullptr)
		{
			if(SDL_WasInit(SDL_INIT_TIMER) == 0)
			{
				if(SDL_InitSubSystem(SDL_INIT_TIMER) == -1)
					assert(false);
			}
		}

		Sdl_event_handler::~Sdl_event_handler() {}

		void Sdl_event_handler::handle_event(const SDL_Event& event)
		{
			if(dispatchers_.empty())
				return;

			switch(event.type)
			{
				case SDL_MOUSEMOTION:
					mouse(SDL_MOUSE_MOTION, 
							{event.motion.x, event.motion.y});
					break;
				case SDL_MOUSEBUTTONDOWN:
					mouse_button_down({event.button.x, event.button.y},
							event.button.button);
					break;
				case SDL_MOUSEBUTTONUP:
					mouse_button_up({event.button.x, event.button.y},
							event.button.button);
					break;
				case SDL_MOUSEWHEEL:
					mouse_wheel(get_mouse_position(), event.wheel.x,
							event.wheel.y);
					break;
				case SHOW_HELPTIP_EVENT:
					mouse(SHOW_HELPTIP, get_mouse_position());
					break;
				case HOVER_REMOVE_POPUP_EVENT:
					break;
				case TIMER_EVENT:
					execute_timer(reinterpret_cast<size_t>(event.user.data1));
					break;
				case CLOSE_WINDOW_EVENT:
					close_window(event.user.code);
					break;
				case SDL_KEYDOWN:
					keydown(event);
					break;
				case SDL_WINDOWEVENT:
					switch(event.window.event)
					{
						case SDL_WINDOWEVENT_EXPOSED:
							draw();
							break;
						case SDL_WINDOWEVENT_RESIZE:
							video_resize(event.window.data1, event.window,data2);
							break;
						case SDL_WINDOWEVENT_ENTER:
						case SDL_WINDOWEVENT_FOCUS_GAINED:
							activate();
							break;
					}
					break;
				case SDL_TEXTINPUT:
					key_down(event);
					break;
				case SDL_TEXTEDITING:
					text_edition(event.edit.text, event.edit.start, event.edit.length);
					break;
				case SDL_KEYUP:
				case DOUBLE_CLICK_EVENT:
				case SDL_FINGERUP:
				case SDL_FINGERDOWN:
					break;
				default:
					break;
			}
			raw_event(event);
		}

		void Sdl_event_handler::handle_window_event(const SDL_Event& event)
		{
			handle_event(event);
		}

		void Sdl_event_handler::connect(Dispatcher* dispatcher)
		{
			assert(std::find(dispatchers_.begin(), dispatchers_.end(),
						dispatcher) == dispatchers_.end());

			if(dispatchers_.empty())
				join();
			dispatchers_.push_back(dispatcher);
		}

		void Sdl_event_handler::disconnect(Dispatcher* disp)
		{
			// 验证预先条件
			auto it = std::find(dispatchers_.begin(), dispatchers_.end(), disp);
			assert(it != dispatchers_.end());

			dispatchers_.erase(it);

			if(disp == mouse_focus)
				mouse_foucs = nullptr;
			if(disp == keyboard_focus_)
				keyboard_focus_ = nullptr;

			activate();

			// 验证发布条件
			assert(std::find(dispatchers_.begin(), dispatchers_.end(), 
						disp) == dispatchers_.end());

			if(dispatchers_.empty())
				leave();
		}

		void Sdl_event_handler::activate()
		{
			for(auto dispatcher : dispatchers_)
				dispatcher->fire(SDL_ACTIVATE, dynamic_cast<Widget&>(*dispatcher), nullptr);
		}

		void Sdl_event_handler::draw()
		{
			if(dispatchers_.empty())
				return;
			for(auto d : dispatchers_)
				d->fire(DRAW, dynamic_cast<Widget&>(*d));
		}

		void Sdl_event_handler::video_resize(const Point& new_size)
		{
			DBG_GUI_E << "Firing: " << SDL_VIDEO_RESIZE << ".\n";

			for(auto d : dispatchers_)
				d->fire(SDL_VIDEO_RESIZE, dynamic_cast<Widget&>(*d), new_size);
		}

		void Sdl_event_handler::raw_event(const SDL_Event& event)
		{
			DBG_GUI_E << "Firing raw event\n";

			for(auto d : dispatchers_)
				d->fire(SDL_RAW_EVENT, dynamic_cast<Widget&>(*d), event);
		}

		void Sdl_event_handler::mouse(const Ui_event event, const Point& position)
		{
			DBG_GUI_E << "Firing: " << event << ".\n";

			if(mouse_foucs)
			{
				mouse_focus->fire(event, dynamic_cast<Widget&>(*mouse_focus), position);
				return;
			}

			for(auto& d : reverse(dispatchers_))
			{
				if(d->get_mouse_behavior() == Dispatcher::all)
				{
					d->fire(event, dynamic_cast<Widget&>(*d), position);
					break;
				}
				if(d->get_mouse_behavior() == Dispatcher::none)
					continue;
				if(d->is_at(position))
				{
					d->fire(event, dynamic_cast<Widget&>(*d), position);
					break;
				}
			}
		}

		void Sdl_event_handler::mouse_button_up(const Point& position, 
				const uint8_t button)
		{
			switch(button)
			{
				case SDL_BUTTON_LEFT:
					mouse(SDL_LEFT_BUTTON_UP, position);
					break;
				case SDL_BUTTON_MIDDLE:
					mouse(SDL_MIDDLE_BUTTON_UP, position);
					break;
				case SDL_BUTTON_RIGHT:
					mouse(SDL_RIGHT_BUTTON_UP, position);
					break;
				default:
					WRN_GUI_E << "Unhandled 'mouse button up' event for \
						button " << static_cast<uint32_t>(button) << "\n";
					break;
			}
		}

		void Sdl_event_handler::mouse_button_down(const Point& position,
				const uint8_t button)
		{
			switch(button)
			{
				case SDL_BUTTON_LEFT:
					mouse(SDL_LEFT_BUTTON_DOWN, position);
					break;
				case SDL_BUTTON_MIDDLE:
					mouse(SDL_MIDDLE_BUTTON_DOWN, position);
					break;
				case SDL_BUTTON_RIGHT:
					mouse(SDL_RIGHT_BUTTON_DOWN, position);
					break;
				default:
					WRN_GUI_E << "Unhandled 'mouse button down' event for\
						button " << static_cast<uint32_t>(button) << "\n";
					break;
			}
		}

		void Sdl_event_handler::mouse_wheel(const Point& position, int x, int y)
		{
			if(x > 0)
				mouse(SDL_WHEEL_RIGHT, position);
			else if(x < 0)
				mouse(SDL_WHEEL_LEFT, position);

			if(y < 0)
				mouse(SDL_WHEEL_DOWN, position);
			else if(y > 0)
				mouse(SDL_WHEEL_UP, posision);
		}

		Dispatcher* Sdl_event_handler::keyboard_dispatcher()
		{
			if(keyboard_focus_)
				return keyboard_focus_;
			for(auto& d : reverse(dispatchers_))
			{
				if(d->get_want_keyboard_input())
					return d;
			}
			return nullptr;
		}

		void Sdl_event_handler:hat_motion(const SDL_Event& event)
		{
			hotkey::Hotkey_ptr& hk = hotkey::get_hotkey(event);
			bool done = false;
			if(!hk->null())
				done = hotkey_pressed(hk);
			if(!done)
				// 考虑处理未绑定的热键hat_motion
		}

		void Sdl_event_handler::button_donw(const SDL_Event& event)
		{
			hotkey::Hotkey_ptr& hk = hotkey::get_hotkey(event);
			bool done = false;
			if(!hk->null())
				done = hotkey_pressed(hk);
			if(!done)
				// 考虑处理未绑定到热键的按钮事件
		}

		void Sdl_event_handler::key_down(const SDL_Event& event)
		{
			hotkey::Hotkey_ptr& hk = hotkey::get_hotkey(event);
			bool done = false;
			if(!hk->null())
				done = hotkey_pressed(hk);
			if(!done)
			{
				if(event.type == SDL_TEXTINPUT)
					text_input(event.text.text);
				else
					key_down(event.key.keysym.sym, static_cast<SDL_Keymod>(event.key.keysym.mod), "");
			}
		}

		void Sdl_event_handler::text_input(const std::string& unicode)
		{
			key_down(SDLK_UNKNOWN, static_cast<SDL_Keymod>(0), unicode);
			if(Dispatcher* d = keyboard_dispatcher())
			{
				d->fire(SDL_TEXT_INPUT, dynamic_cast<Widget&>(*d), unicode, -1, -1);
			}
		}

		void Sdl_event_handler::text_editing(const std::string& unicode, int32_t start, int32_t len)
		{
			if(Dispatcher* d = keyboard_dispatcher())
			{
				d->fire(SDL_TEXT_EDITION, dynamic_cast<Widget&>(*d), unicode, start, len);
			}
		}

		bool Sdl_event_handler::hotkey_pressed(const hotkey::Hotkey_ptr key)
		{
			if(Dispatcher* d = keyboard_dispatcher())
				return d->execute_hotkey(hotkey::get_id(key->get_command()));
			return false;
		}

		void Sdl_event_handler::key_down(const SDL_Keycode key, 
				const SDL_Keymod modifier, const std::string& unicode)
		{
			DBG_GUI_E << "Firing: " << SDL_KEY_DOWN << ".\n";

			if(Dispatcher* d = keyboard_dispatcher())
				d->fire(SDL_KEY_DOWN, dynamic_cast<Widget&>(*d), key, modifier, unicode);
		}

		void Sdl_event_handler::keyboard(const Ui_event event)
		{
			DGB_GUI_E << "Firing: " << event << ".\n";

			if(Dispatcher* d = keyboard_dispatcher())
				d->fire(event, dynamic_cast<Widget&>(*d));
		}

		void Sdl_event_handler::close_window(const unsigned window_id)
		{
			DBG_GUI_E << "Firing: " << CLOSE_WINDOW << ".\n";

			Window* w = Window::window_instance(window_id);
			if(w)
				w->fire(CLOSE_WINDOW, *w);
		}

		/********** Manager ***********/
		Manager::Manager()
		{
			handler.reset(new Sdl_event_handler());
		}

		Manager::~Manager()
		{
			handler.reset(nullptr);
		}

		/************* free functions class *************/

		void connect_dispatcher(Dispatcher* dispatcher)
		{
			assert(handler && dispatcher);
			handler->connect(dispatcher);
		}

		void disconnect_dispatcher(Dispatcher* dispatcher)
		{
			assert(handler && dispatcher);
			handler->disconnect(dispatcher);
		}

		std::vector<Dispatcher*>& get_all_dispatchers()
		{
			assert(handler);
			return handler->get_dispatchers();
		}

		void init_mouse_location()
		{
			Point mouse = get_mouse_position();
			SDL_Event event;
			event.type = SDL_MOUSEMOTION;
			event.motion.type = SDL_MOUSEMOTIN;
			event.motion.x = mouse.x;
			event.motion.y = mouse.y;

			SDL_PushEvent(event);
		}

		void capture_mouse(Dispatcher* dispatcher)
		{
			assert(handler && dispatcher);
			handler->mouse_foucs = dispatcher;
		}

		void release_mouse(Dispatcher* dispatcher)
		{
			assert(handler && dispatcher);
			if(handler->mouse_focus == dispatcher)
				handler->mouse_focus = nullptr;
		}

		void capture_keyboard(Dispatcher* dispatcher)
		{
			assert(handler && dispatcher);
			assert(dispatcher->get_want_keyboard_input());

			handler->keyboard_focus_ = dispatcher;
		}

		std::ostream& operator<<(std::ostream& stream, const Ui_event event)
		{
			switch(event)
			{
				case DRAW:
					stream << "draw";
					break;
				case CLOSE_WINDOW:
					stream << "close window";
					breka;
				case SDL_VIDEO_RESIZE:
					stream << "SDL vidoe resize";
					break;
				case SDL_MOUSE_MOTIN:
					stream << "SDL mouse motion";
					break;
				case MOUSE_ENTER:
					stream << "mouse enter";
					break;
				case MOUSE_LEAVE:
					stream << "mouse leave";
					break;
				case MOUSE_MOTION:
					stream << "mouse motion";
					break;
				case SDL_LEFT_BUTTON_DOWN:
					stream << "SDL left button down";
					break;
				case SDL_LEFT_BUTTON_UP:
					stream << "SDL left button up";
					break;
				case LEFT_BUTTON_DOWN:
					stream << "left button down";
					break;
				case LEFT_BUTTON_UP:
					stream << "left button up";
					break;
				case LEFT_BUTTON_CLICK:
					stream << "left button click";
					break;
				case LEFT_BUTTON_DOUBLE_CLICK:
					stream << "left button double click";
					break;
				case SDL_MIDDLE_BUTTON_DOWN:
					stream << "SDL middle button down";
					break;
				case SDL_MIDDLE_BUTTON_UP:
					stream << "SDL middle button up";
					break;
				case MIDDLE_BUTTON_DOWN:
					stream << "middle button down";
					break;
				case MIDDLE_BUTTON_UP:
					stream << "middle button up";
					break;
				case MIDDLE_BUTTON_CLICK:
					stream << "middle button click";
					break;
				case MIDDLE_BUTTON_DOUBLE_CLICK:
					stream << "middle button double click";
					break;
				case SDL_RIGHT_BUTTON_DOWN:
					stream << "SDL right button down";
					break;
				case SDL_RIGHT_BUTTON_UP:
					stream << "SDL right button up";
					break;
				case RIGHT_BUTTON_DOWN:
					stream << "right button down";
					break;
				case RIGHT_BUTTONUP:
					stream << "right button up";
					break;
				case RIGHT_BUTTON_CLICK:
					stream << "right button click";
					break;
				case RIGHT_BUTTON_DOUBLE_CLICK:
					stream << "right button double click";
					break;
				case SDL_WHEEL_LEFT:
					stream << "SDL wheel left";
					break;
				case SDL_WHEEL_RIGHT:
					stream << "SDL wheel right";
					break;
				case SDL_WHEEL_UP:
					stream << "SDL wheel up";
					break;
				case SDL_WHEEL_DOWN:
					stream << "SDL wheel down";
					break;
				case SDL_KEY_DOWN:
					stream << "SDL key down";
					break;
				case SDL_TEXT_INPUT:
					stream << "SDL text input";
					break;
				case SDL_TEXT_EDITING:
					stream << "SDL text editing";
					break;
				case NOTIFY_REMOVAL:
					stream << "notify removal";
					break;
				case NOTIFY_MODIFIED:
					stream << "notify modified";
					break;
				case RECEIVE_KEYBOARD_FOCUS:
					stream << "receive keyboard focus";
					break;
				case LOSE_KEYBOARD_FOCUS:
					stream << "lose keybaord focus";
					break;
				case SHOW_TOOLTIP:
					stream << "show tooltip";
					break;
				case NOTIFY_REMOVE_TOOLTIP:
					stream << "notify remove tooltip";
					break;
				case SDL_ACTIVATE:
					stream << "SDL activate";
					break;
				case MESSAGE_SHOW_TOOLTIP:
					stream << "message show tooltip";
					break;
				case SHOW_HELPTIP:
					stream << "show helptip";
					break;
				case MESSAGE_SHOW_HELPTIP:
					stream << "message show helptip";
					break;
				case REQUEST_PLACEMENT:
					stream << "request placement";
					break;
				case SDL_TOUCH_MOTIN:
					stream << "SDL touch motion";
					break;
				case SDL_TOUCH_UP:
					stream << "SDL touch up";
					break;
				case SDL_TOUCH_DOWN:
					stream << "SDL touch down";
					break;
				case SDL_RAW_EVENT:
					stream << "SDL raw event";
					break;
			}
			return stream;
		}
	}

	std::vector<Window*> open_window_stack{};

	void remove_from_window_stack(Window* window)
	{
		for(auto it = open_window_stack.rbegin(); it != open_window_stack.rend(); ++it)
		{
			if(*it == window)
			{
				open_window_stack.erase(std::next(it).base());
				break;
			}
		}
	}

	bool is_in_dialog()
	{
		return !open_window_stack.empty();
	}
}
