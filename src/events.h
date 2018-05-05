/*
 * Copyright (C) 2017 by Shan Daming
 */

#ifndef EVENTS_H
#define EVENTS_H

#include <vector>
#include <list>
#include <functional>
#include <SDL2/events.h>

// 自定义双击事件类型
#define DOUBLE_CLICK_EVENT SDL_USEREVENT
#define TIMER_EVENT (SDL_USEREVENT + 1)
#define HOVER_REMOVE_POPUP_EVENT (SDL_USEREVENT + 2)
#define DRAW_EVENT (SDL_USEREVENT + 3)
#define CLOSE_WINDOW_EVENT (SDL_USEREVENT + 4)
#define SHOW_HELPTIP_EVENT (SDL_USEREVENT + 5)
#define DRAW_ALL_EVENT (SDL_USEREVENT + 6)
#define INVOKE_FUNCTION_EVENT (SDL_USEREVENT + 7)

namespace events
{
	class Sdl_handler;
	typedef std::list<Sdl_handler*> Handler_list;

        struct Context
        {
	        Context() : handlers(), focused_handler(handlers.end()), 
		        staging_handlers() {}
	        ~Context();

	        void add_handler(Sdl_handler* ptr);
	        bool remove_handler(Sdl_handler* ptr);
	        void cycle_focus(); // 循环重点
	        void set_focus(const Sdl_handler* ptr);
	        void add_staging_handlers(); // 添加分段处理程序

	        Handler_list handlers;
	        Handler_list::iterator focused_handler; // 重点处理程序
	        std::vector<Sdl_handler*> staging_handlers;
        };

        /*
         * 任何派生自这个类都会在其生命周期中通过句柄自动接受sdl事件，而创建
		 * 他们的事件上下文是活动的。
		 * 注意：必须在处理程序对象初始化之前初始化event_context对象，并且在
		 * 处理程序销毁后必须销毁event_context。
		 */
        class Sdl_handler
        {
		        friend class Context;
	        public:
		        virtual void handle_event(const SDL_Event& event) = 0;
		        virtual void handle_window_event(const SDL_Event& event) = 0;
		        virtual void process_event() {}
		        virtual void draw() {}

		        virtual void volatile_draw() {} // 易变的绘图
		        virtual void volatile_undraw() {}

		        virtual bool requires_event_focus(const SDL_Event* e = nullptr) 
			        const { return false; }

		        virtual void process_help_string(int /*mousex*/, int /*mousey*/) {}
		        virtual void process_tooltip_string(int /*mousex*/, 
				        int /*mousey*/) {}

		        virtual void join(); // 加入当前事件上下文。
		        virtual void join(Context& c); // 加入指定的事件上下文

		        // joins the same event context as the parent is already 
				// associated widh
		        virtual void join_same(Sdl_handler* parent);
		        virtual void leave(); // leave the event context.

		        virtual void join_global(); // join the global event context.
		        virtual void leave_global(); // leave the global event context.

		        virtual bool has_joined() { return has_joined_; }
		        virtual bool has_joined_global() { return has_joined_global_; }
	        protected:
		        Sdl_handler(const bool auto_join = true);
		        virtual ~Sdl_handler();

		        virtual std::vector<Sdl_handler*> handler_members()
		        {
			        return std::vector<Sdl_handler*>();
		        }
	        private:
		        bool has_joined_;
		        bool has_joined_global_;
        };

        void focus_handler(const Sdl_handler* ptr);

        bool has_focus(const Sdl_handler* ptr, const SDL_Event* event);

        void call_in_main_thread(const std::function<void(void)>& f);

        typedef std::vector<SDL_handler*> sdl_handler_vector;

        /*
         * Event_context对象控制SDL事件发送到的处理程序对象，当创建
		 * Event_context时，它将成为当前事件上下文，Event_context对象必须以
		 * LIFO排序方式互相创建，并且与处理程序员对象相关，也就是说，所有的
		 * Event_context对象都应该创建为自动/堆栈变量。
		 * 处理程序对象不需要创建为自动变量（例如可以将他们放入向量里），但
		 * 你必须确保处理程序对象在其上下文被销毁时销毁
		 */
        struct Event_context
        {
	        Event_context();
	        ~Event_context();
        };

		// 使所有事件分派给所有处理对象
		void run_event_loop();

        // look for resize events and update references to the screen area
        void peek_for_resize(); // 偷看调整大小

		// 泵信息
        struct Pump_info 
        {
		        Pump_info() : resize_dimensions(), ticks_(0) {}
		        std::pair<int, int> resize_dimensions; // 调整尺寸
		        int ticks(unsigned* refresh_counter = nullptr, 
				        unsigned refresh_rate = 1);
	        private:
		        int ticks_; // 0 if not calculated
        };

		// 泵监视器
        class Pump_monitor
        {
			// Pump_monitor在发生events::run_event_loop()后接到通知
	        public:
		        Pump_monitor();
		        virtual ~Pump_monitor();
		        virtual void process(Pump_info& info) = 0;
        };

        void initialise();
		// 敲定
        void finalize();

		// 提高...事件
        void raise_process_event();
        void raise_resize_event();
        void raise_draw_event();
        void raise_draw_all_event();
        void raise_volatile_draw_event();
        void raise_volatile_draw_all_event();
        void raise_volatile_undraw_event();
        void raise_help_string_event(int mousex, int mousey);

        /*
         * Is the event an input event ?
         *
         * @returns whether or not the event is an input event
         */
        bool is_input(const SDL_Event& event);

        // Discards丢弃 all input events
        void discard_input();
}

#endif
