/*
 * Copyright (C) 2018
 */

#include "distributor.h"

namespace gui
{
	namespace event
	{
		/*
		 * 小型助手保持资源被锁住
		 *
		 * 一些事件处理例程不能被递归调用，这是由于他们被附加到预先队列以及
		 * 当预先队列事件被递归触发时转发一个事件导致无限递归的事实。
		 *
		 * 为防止这些功能在锁定时锁定并退出，否则请在此处抓住锁定。
		 */
		class Resource_locker
		{
			public:
				Resource_locker(bool& locked) : locked_(locked)
				{
					assert(!locked_);
					locked_ = true;
				}
				
				~Resource_locker()
				{
					assert(locked_);
					locked_ = false;
				}
			private:
				bool& locked_;
		};

		/********************** Mouse_motion ************************/

#define LOG_HEADER "distributor mouse motion [" << owner_.id() << "]: "

		Mouse_motion::Mouse_motion(Widget& owner, const Dispatcher::Queue_postion queue_position) : mouse_focus_(false), mouse_captured_(false),
		owner_(owner), hover_timer_(0), hover_postion_(0, 0),
		hover_shown_(true), hover_widget_(nullptr), 
		signal_handler_sdl_mouse_motion_entered_(false)
		{
			owner.connect_signal<event::SDL_MOUSE_MOTION>(std::bind(&Mouse_motion::signal_handler_sdl_mouse_motion, this, _2, _3, _5), queue_position);

			owner_.connect_signal<event::SDL_WHEEL_UP>(std::bind(&Mouse_motion::signal_handler_sdl_wheel, this, _2, _3, _5));
			owner_.connect_signal<event::SDL_WHEEL_DOWN>(std::bind(&Mouse_motion::signal_handler_sdl_wheel, this, _2, _3, _5));
			owner_.connect_signal<event::SDL_WHEEL_LEFT>(std::bind(&Mouse_motion::signal_handler_sdl_wheel, this, _2, _3, _5));
			owner_.connect_signal<event::SDL_WHEEL_RIGHT>(std::bind(&Mouse_motion::signal_handler_sdl_wheel, this, _2, _3, _5));

			owner.connect_signal<event::SHOW_HELPTIP>(std::bind(&mouse_motion::signal_handler_show_helptip, this, _2, _3, _5), queue_position);
		}

		Mouse_motion::~Mouse_motion()
		{
			stop_hover_timer();
		}

		void Mouse_motion::caption_mouse(const bool capture)
		{
			assert(mouse_focus_);
			mouse_captured_ = capture;
		}

		void Mouse_motion::signal_handler_sdl_mouse_motion(const event::Ui_event event, bool& handled, const Point& coordinate)
		{
			if(signal_handler_sdl_mouse_motion_entered_)
				return;
			Resource_locker lock(signal_handler_sdl_mouse_motion_entered_);

			DBG_GUI_E << LOG_HEADER << event << ".\n";

			if(mouse_captured_)
			{
				assert(mouse_focus_);
				if(!ovner_.fire(event, *mouse_focus_, coordinate))
					mouse_hover(mouse_focus_, coordinate);
			}
			else
			{
				Widget* mouse_over = owner_.find_at(coordinate, true);
				while(mouse_over && !mouse_over->can_mouse_focus() && 
						mouse_over->parent())
				{
					mouse_over = mouse_over->parent();
				}
				if(mouse_over)
				{
					DBG_GUI_E << LOG_HEADER << "Firing: " << event << ".\n";
					if(owner_.fire(event, *mouse_over, coordinate))
						return;
				}

				if(!mouse_focus_ && mouse_over)
					mouse_enter(mouse_over);
				else if(mouse_focus_ && !mouse_over)
					mouse_leave();
				else if(mouse_focus_ && mouse_foucs_ == mouse_over)
					mouse_hover(mouse_over, coordinate);
				else if(mouse_foucs_ && mouse_over)
				{
					// moved from one widget to the next
					mouse_leave();
					mouse_enter(mouse_over);
				}
				else
					assert(!mouse_focus_ && !mouse_over);
			}
			handled = true;
		}

		void Mouse_motion::signal_handler_sdl_wheel(const event::Ui_event event, bool& handled, const Point& coordinate)
		{
			DBG_GUI_E << LOG_HEADER << event << ".\n";

			if(mouse_captured_)
			{
				assert(mouse_focus_);
				owner_.fire(event, *mouse_focus_, coordinate);
			}
			else
			{
				Widget* mouse_over = owner_.find_at(coordinate, true);
				if(mouse_over)
					owner_.fire(event, *mouse_over, coordinate);
			}
			handled = true;
		}

		void Mouse_motion::signal_handler_show_helptip(const event::Ui_event event, bool& handled, const Point& coordinate)
		{
			DBG_GUI_E << LOG_HEADER << event << ".\n";

			if(mouse_captured_)
			{
				assert(mouse_focus_);
				if(owner_.fire(event, *mouse_focus_, coordinate))
					stop_hover_timer();
			}
			else
			{
				Widget* mouse_over = owner_.find_at(cooridinate, true);
				if(mouse_over)
				{
					DBG_GUI_E << LOG_HEADER << "Firing: " << event << ".\n";
					if(owner_.fire(event, *mouse_over, coordinate))
						stop_hover_timer();
				}
			}

			handled = true;
		}
	}
}
