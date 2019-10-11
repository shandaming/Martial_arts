/*
 * Copyright (C) 2018
 */

#include "dispatcher.h"
#include "events.h"

namespace gui
{
	namespace event
	{
		struct Dispatcher_implementation
		{
			/*
			 * @param SET	事件类型需要的集合，例如gui::event::set_event或
			 *				在该标题中定义的类似集合
			 * @param FUNCTION	函数签名验证函数SFINAE是否针对例如
			 *					gui::event::Signal_function或该头文件中里一
			 *					个函数签名.
			 * @param QUEUE	插入事件的队列
			 */
#define IMPLEMENT_EVENT_SIGNAL(SET, FUNCTION, QUEUE)	\
			/*													
			 * 返回FUNCTION的信号结构
			 * 有几个函数只会重载返回值，以便它们使用SFINAE
			 */												\
			template<typename F>							\
			static std::enable_if_t<std::is_same<F, FUNCTION>::value, Dispatcher::Signal_type<FUNCTION>>&								\
			event_signal(Dispatcher& dispatcher, const Ui_event event)	\
			{														\
				return dispatcher.QUEUE.queue[event];				\
			}														\
			IMPLEMENT_EVENT_SIGNAL(set_event, Signal_function, signal_queue_)

			/*
			 * @ref IMPLEMENT_EVENT_SIGNAL的包装助手宏
			 * 由于参数@ref IMPLEMENT_EVENT_SIGNAL使用相同的参数，每种类型
			 * 略有不同，因此此宏按其包装该函数。
			 */
#define IMPLEMENT_EVENT_SIGNAL_WRAPPER(TYPE)					\
				IMPLEMENT_EVENT_SIGNAL(set_event_##TYPE, \
						signal_##TYPE##_function,		\
						signal_##TYPE##_queue_)

				IMPLEMENT_EVENT_SIGNAL_WRAPPER(mouse)
				IMPLEMENT_EVENT_SIGNAL_WRAPPER(keyboard)
				IMPLEMENT_EVENT_SIGNAL_WRAPPER(touch)
				IMPLEMENT_EVENT_SIGNAL_WRAPPER(notification)
				IMPLEMENT_EVENT_SIGNAL_WRAPPER(message)
				IMPLEMENT_EVENT_SIGNAL_WRAPPER(raw_event)
				IMPLEMENT_EVENT_SIGNAL_WRAPPER(text_input)

#undef IMPLEMENT_EVENT_SIGNAL_WRAPPER
#undef IMPLEMENT_EVENT_SIGNAL

#define IMPLEMENT_RUNTIME_EVENT_SIGNAL_CHECK(TYPE)		\
				else if(is_##TYPE##_event(event))		\
					return queue_check(dispatcher.signal_##TYPE##_queue_);

				// 测试调度程序是否具有特定事件的处理程序的帮助程序
				static bool has_handler(Dispatcher& dispatcher,
						const Dispatcher::Event_queue_type queue_type,
						Ui_event event)
				{
					const auto queue_check = [&](auto& queue_set)
					{
						return !queue_set.queue[event].empty(queue_type);
					};

					if(is_general_event(event))
						return queue_check(dispatcher.signal_queue_);

					IMPLEMENT_RUNTIME_EVENT_CHECK(mouse)
					IMPLEMENT_RUNTIME_EVENT_CHECK(keyboard)
					IMPLEMENT_RUNTIME_EVENT_CHECK(touch)
					IMPLEMENT_RUNTIME_EVENT_CHECK(notification)
					IMPLEMENT_RUNTIME_EVENT_CHECK(message)
					IMPLEMENT_RUNTIME_EVENT_CHECK(raw_event)
					IMPLEMENT_RUNTIME_EVENT_CHECK(text_input)

					return false;
				}
#undef IMPLEMENT_RUNTIME_EVENT_SIGNAL_CHECK
		};
		
		namespace implementation
		{
			/*
			 * 构建事件链
			 * 事件链是从小部件的第一个父节点开始直到（并包括）通缉父节点的
			 * 一系列事件，对于所有这些小部件，将测试它们是否具有该事件的
			 * pre or post handler.通过这种方式，将会有小部件列表尝试发送
			 * 事件，如果从小部件到父级没有连线，则结果为定义。（如果widget
			 * == dispatcher的结果将永远是空的）。
			 */
			template<typename T>
			inline std::vector<std::pair<Widget*, Ui_event>>
			build_event_chain(const Ui_event event, Widget* dispatcher,
					Widget* w)
			{
				assert(dispatcher && w);

				std::vector<std::pair<Widget*, Ui_event>> result;
				while(1)
				{
					if(w->has_event(event, Dispatcher::Event_queue_type(Dispatcher::pre | Dispatcher::post)))
						result.emplace_back(w, event);
					if(w == dispatcher)
						break;
					w = w->parent();
					assert(w);
				}
				return result;
			}

			/*
			 * 构建Signal_notification_function事件链
			 * 通知只发送给接收者，它返回一个空链，由于前后队列未被使用，它
			 * 会验证它们是否为空（使用断言）
			 */
			template<>
			inline std::vector<std::pair<Widget*, Ui_event>>
			build_event_chain<Signal_notification_function>(const Ui_event event, Widget* dispatcher, Widget* w)
			{
				assert(dispatcher && w);
				assert(!w->has_event(event, Dispatcher::Event_queue_type(Dispatcher::pre | Dispatcher::post)));

				return std::vector<std::pair<Widget*, Ui_event>>();
			}

			/*
			 * 构建Signal_message_function的事件链
			 * 此函数期望发送它的小部件也是接收者，这个假设可能会改变，但现
			 * 在是有效的，该函数不会构建@p dispatcher到@p小部件的事件链，而
			 * 从@p小部件到其顶级项目（第一个没有父级的项目），我们称为@p
			 * 窗口。
			 */
			template<>
			inline std::vector<std::pair<Widget*, Ui_event>>
			build_event_chain<Signal_message_function>(const Ui_event event,
					Widget* dispatcher, Widget* w)
			{
				assert(dispatcher && w && w == dispatcher);

				std::vector<std::pair<Widget*, Ui_event>> res;
				// 我们只将小部件的父对象添加到链中
				while(w == w->parent())
				{
					assert(w);
					if(w->has_event(event, Dispatcher::Event_queue_type(Dispatcher::pre | Dispatcher::post)))
						res.emplace(res.begin(), w, event);
				}
				return res;
			}

			/*
			 * fire_event辅助函数
			 * 除了event_chain外，它的参数与fire_event参数相同，event_chain
			 * 包含带有调用事件的小部件
			 */
			template<typename T, typename... F>
			inline bool fire_event(const Ui_event event,
					std::vector<std::pair<Widget*, Ui_event>>& event_chain,
					Widget* dispatcher, Widget* w, F&&... params)
			{
				bool handled = false;
				bool halt = false;

				// pre
				for(auto& rit_widget : reverse(event_chain))
				{
					auto& signal = Dispatcher_implementation::event_signal<T>(*rit_widget.first, rit_widget.second);
					for(auto& pre_func : signal.pre_child)
					{
						pre_func(*dispatcher, rit_widget.second, handled, halt, std::forward<F>(params)...);
						if(halt)
						{
							assert(handled);
							break;
						}
					}
					if(handled)
						return true;
				}
				// child
				if(w->has_event(event, Dispatcher::child))
				{
					auto& signal = Dispatcher_implementation::event_signal<T>(*w, event);
					for(auto& func : signal.child)
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
				// post
				for(auto& it_widget : event_chain)
				{
					auto& signal = Dispatcher_implementation::event_signal<T>(*it_widget.first, it_widget.second);
					for(auto& post_func : signal.post_child)
					{
						post_func(*dispatcher, it_widget.second, handled, halt, std::forward<F>(params)...);
						if(halt)
						{
							assert(handled);
							break;
						}
					}
					if(handled)
						return true;
				}
				//unhandled
				assert(handled == false);
				return false;
			}
		}

		/*
		 * 出发一个事件
		 * 允许不同信号功能之间共享通用事件触发代码
		 */
		template<typename T, typename... F>
		fire_event(const Ui_event event, Dispatcher& d, Widget* w, F&&... params)
		{
			assert(d && w);

			Widget* dispatcher_w = dynamic_cast<Widget*>(d);
			std::vector<std::pair<Widget*, Ui_event>> event_chain = implementation::build_event_chain<T>(event, dispatcher_w, w);

			return implementation::fire_event<T>(event, event_chain, dispatcher_w, w, std::forward<F>(params)...);
		}

		template<Ui_event click, Ui_event double_click, bool(Event_executor::*want_double_click)() const, typename T, typename... F>
		inline bool fire_event_double_click(Dispatcher* dsp, Widget* wgt, F&&... params)
		{
			assert(dsp && wgt);

			std::vector<std::pair<Widget*, Ui_event>> event_chain;
			Widget* w = wgt;
			Widget* d = dynamic_cast<Widget*>(dsp);

			while(w != d)
			{
				w = w->parent();
				assert(w);
				if((w->want_double_click)())
				{
					if(w->has_event(double_click, Dispatcher::Event_queue_type(Dispatcher::pre | Dispatcher::post)))
						event_chain.emplace_back(w, double_click);
				}
				else
				{
					if(w->has_event(click, Dispatcher::Event_queue_type(Dispatcher::pre | Dispatcher::post)))
						event_chain.emplace_back(w, click);
				}
			}

			if((wgt->*wants_double_click)())
				return implementation::fire_event<T>(double_click, event_chain, d, wgt, std::forward<F>(params)...);
			else
				return implementation::fire_event<T>(click, event_chain, d, wgt, std::forward<F>(params)...);
		}
	}
}
