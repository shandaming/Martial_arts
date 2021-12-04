/*
 * Copyright (C) 2018
 */

#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <list>
#include <map>
#include <type_traits>
#include <SDL2/SDL_events.h>
#include "handler.h"
#include "hotkey_command.h"

namespace gui
{
	namespace event
	{
		/*
		 * 帮助捕捉@ref_dispatcher::connect_signal使用错误
		 * 由于用户无法向Dispatcher::connect_signal提供错误的回调函数，因此
		 * 需要此帮助程序，如果发送错误的回调，他将永远不会被调用。
		 * 此版本用于没有额外参数的回调，注意一些像MOUSE_ENTER这样的鼠标功能
		 * 不会将鼠标坐标发送到回调函数，因此他们也属于这个类别.
		 */
		constexpr bool is_general_event(const Ui_event event)
		{
                        return event == DRAW
		                || event == CLOSE_WINDOW
		                || event == MOUSE_ENTER
		                || event == MOUSE_LEAVE
		                || event == LEFT_BUTTON_DOWN
		                || event == LEFT_BUTTON_UP
		                || event == LEFT_BUTTON_CLICK
		                || event == LEFT_BUTTON_DOUBLE_CLICK
		                || event == MIDDLE_BUTTON_DOWN
		                || event == MIDDLE_BUTTON_UP
		                || event == MIDDLE_BUTTON_CLICK
		                || event == MIDDLE_BUTTON_DOUBLE_CLICK
		                || event == RIGHT_BUTTON_DOWN
		                || event == RIGHT_BUTTON_UP
		                || event == RIGHT_BUTTON_CLICK
                                || event == RIGHT_BUTTON_DOUBLE_CLICK;
		}

		// 此版本用于具有坐标作为额外参数的回调
		constexpr bool is_mouse_event(const Ui_event event)
		{
	                return event == SDL_VIDEO_RESIZE
		                || event == SDL_MOUSE_MOTION
		                || event == MOUSE_MOTION
		                || event == SDL_LEFT_BUTTON_DOWN
		                || event == SDL_LEFT_BUTTON_UP
		                || event == SDL_MIDDLE_BUTTON_DOWN
		                || event == SDL_MIDDLE_BUTTON_UP
		                || event == SDL_RIGHT_BUTTON_DOWN
		                || event == SDL_RIGHT_BUTTON_UP
		                || event == SHOW_TOOLTIP
		                || event == SHOW_HELPTIP
		                || event == SDL_WHEEL_UP
		                || event == SDL_WHEEL_DOWN
		                || event == SDL_WHEEL_LEFT
                                || event == SDL_WHEEL_RIGHT;
                }

		// 此版本用于键盘值的回调（还未确定）
		constexpr bool is_keyboard_event(const Ui_event event)
		{
			return event == SDL_KEY_DOWN;
		}

		// 此版本用于触摸事件的回调
		constexpr bool is_touch_event(const Ui_event event)
		{
			return event == SDL_TOUCH_MOTION ||
				event == SDL_TOUCH_UP ||
				event == SDL_TOUCH_DOWN;
		}

		// 这版本用于发件人又名通知消息的回调，就像set_event中的那样，它没有
		// 额外的参数，但这版本只发送给目标，不使用前后队列
		constexpr bool is_notification_event(const Ui_event event)
		{
	                return event == NOTIFY_REMOVAL
		                || event == NOTIFY_MODIFIED
		                || event == RECEIVE_KEYBOARD_FOCUS
		                || event == LOSE_KEYBOARD_FOCUS
		                || event == NOTIFY_REMOVE_TOOLTIP
                                || event == SDL_ACTIVATE;
		}

		// 这版本用于发件人又名通知消息的回调，与通知不同，该消息通过链发送
		// ，事件从窗口小部件一直发送到窗口，窗口始终是消息的接受者，（除非
		// 有人在之前抓住它）
		constexpr bool is_message_event(const Ui_event event)
		{
			return event == MESSAGE_SHOW_TOOLTIP ||
				event == MESSAGE_SHOW_HELPTIP ||
				event == REQUEST_PLACEMENT;
		}

		constexpr bool is_raw_event_event(const Ui_event event)
		{
			return event == SDL_RAW_EVENT;
		}

		constexpr bool is_text_input_event(const Ui_event event)
		{
			return event == SDL_TEXT_INPUT || event == SDL_TEXT_EDITING;
		}

		/*
		 * 回调函数签名
		 * 有几种回调签名，它只有所有回调共享的参数
		 * 此函数用于set_event中的回调
		 */
		typedef std::function<void(Widget& dispatcher, const Ui_event evnet,
				bool& handled, bool& halt)> Signal_function;

		// 此函数用于set_event_mouse
		typedef std::function<void(Widget& dispatcher, const Ui_event event,
				bool& handled, bool& halt, 
				const Point& coordinate)> Signal_mouse_function;

		// 此函数用于set_event_keyboard
		typedef std::function<void(Widget& dispatcher, const Ui_event event,
				bool& handled, bool& halt, const SDL_Keycode key,
				const SDL_Keymod modifier, 
				const std::string& unicode)> Signal_keyboard_function;

		// 此函数用于set_event_touch
		typedef std::function<void(Widget& dispatcher, const Ui_event event,
				bool& handled, bool& halt, const Point& pos,
				const Point& distance)> Signal_touch_function;

		/*
		 * 此函数用于set_event_notificatin中的回调，添加了虚拟void*参数，该
		 * 参数将为nullptr以获取与Signal_function的回调不同的签名。
		 */
		typedef std::function<void(Widget& dispatcher, const Ui_event event,
				bool& handled, bool& halt, 
				void*)> Singal_notification_function;

		// 此函数用于set_event_message
		typedef std::function<void(Widget& dispatcher, const Ui_event event,
				bool& handled, bool& halt, 
				Message& message)> Signal_message_function;

		// 此函数用于set_event_raw_event
		typedef std::function<void(Widget& dispatcher, const Ui_event event,
				bool& handled, bool& halt, 
				const SDL_Event& sdlevent)> Signal_raw_event_function;

		// 此函数用于set_event_text_input
		typedef std::function<void(Widget& dispatcher, const Ui_event event,
				bool& handled, bool& halt, const std::string& text,
				int32_t current_pos, 
				int32_t select_len)> Signal_text_input_function;

		// 热键功能处理程序签名
		typedef std::function<void(Widget& dispatcher, 
				hotkey::Hotkey_command id)> Hotkey_function;

		/*
		 * 事件处理基类
		 * 调度员具有事件插槽，当事件到达时，它会查找为该事件注册自己的函数
		 * 并调用其回调函数。
		 * 这个类是所有小部件的基类[1]，小部件在回调中的作用可能差别很大，
		 * 图像可能会忽略所有事件，一个窗口可以跟踪鼠标位置并将MOUSE_ENTER和
		 * MOUSE_LEAVE事件出发到所有涉及的小部件。
		 * [1]不太确定它是否将成为widget或styled_widget的基类
		 */
		class Dispatcher
		{
				friend struct Dispatcher_implementation;
			public:
				/*
				 * 将调度程序连接到事件处理程序
				 * 当调度程序连接到事件处理程序时，它将直接从事件处理程序获
				 * 取事件，对于像Windows这样的顶级项目而言，这是需要的，但
				 * 对于大多数其他小部件而言则不然。
				 * 所以一个窗口可以调用connect来注册自己，它会在销毁后自动
				 * 断开连接。
				 */
				void connect();

				/*
				 * 确定位置是否位于活动窗口小部件中。
				 * 这用于查看鼠标事件是否位于小部件内。
				 */
				virtual bool is_at(const Point& coordinate) const = 0;

				enum Event_queue_type
				{
					pre = 1,
					child = 2,
					post = 4
				};

				bool has_event(const Ui_event event, const Event_queue_type event_type);

				// 触发一个没有额外参数的事件
				bool fire(const Ui_event event, Widget& target);

				/*
				 * 触发一个坐标参数事件
				 * @event	触发的事件
				 * @target	接受事件的小部件
				 * @coordinate	事件鼠标位置
				 */
				bool fire(const Ui_event event, Widget& target, const Point& coordinate);

				/*
				 * 触发一个需要键盘参数的事件
				 * @event 触发的事件
				 * @target 接受触发的小部件
				 * @key		按下的SDL键盘码
				 * @modifier	SDL键盘修饰符
				 * @unicode		按键的unicode值
				 */
				bool fire(const Ui_event event, Widget& target, const SDL_Keycode key, const SDL_Keymod modifier, const std::string& unicode);

				// 触发一个需要触摸参数的事件
				bool fire(const Ui_event event, Widget& target, const Point& pos, const Point& distance);

				/*
				 * 触发通知参数事件
				 * 注意void*参数是SFINAE所需的虚拟对象
				 * @param event 触发事件
				 * @param target 接受事件的部件
				 */
				bool fire(const Ui_event event, Widget& target, void*);

				// 触发消息参数的事件
				bool fire(const Ui_event event, Widget& target, Message& msg);

				// 触发一个sdl原始事件
				bool fire(const Ui_event event, Widget& target, const SDL_Event& sdlevent);

				// 触发一个文本输入事件
				bool fire(const Ui_event event, Widget& target, const std::string& text, int32_t start, int32_t len);

				/*
				 * 在信号处理程序中添加新回调的位置
				 * 信号处理程序中有3个回调队列：pre_child这回调在容器小部件
				 * 发送给子项之前被调用，没有孩子的小工具也应该使用这队列，
				 * 子对象调用正确子对象的回调，post_child父容器在孩子之后被
				 * 调用的回调函数。
				 * 对于每个队列，都可以在前面或后面添加事件。
				 * 是否所有3个队列都执行取决于回调是否修改处理和停止标签，当
				 * 标签被设置时，当前队列执行停止，当这样做时，处理标志也必
				 * 须被设置，当处理标志被设置时，该队列中的事件被执行并且之
				 * 后不再有队列。
				 * 以下是一些用户实例。播放声音并执行可选用户回调的按钮：调
				 * 用按钮内部的点击处理程序并设置处理标志用户安装的回调位于
				 * 同一队列中，并在之后执行。
				 * 切换按钮可以切换或不切换：用户插入回调，验证是否允许该动
				 * 作，如果并不允许则设置停止标志并处理，否则保持标志不变，
				 * 正常的按钮切换功能，然后可能会被调用，如果是这样处理标志
				 * ，可选的，在此处调用里一个用户回调。
				 */
				enum Queue_postion
				{
					front_pre_child,
					back_pre_child,
					front_child,
					back_child,
					front_post_child,
					back_post_child
				};

				// 在set_event中用于回调的信号
				// 该函数使用enable_if来避免注册错误的函数，使用此函数的常见
				// 方法：widget->connect_signal<EVENT_ID>(std::bind(
				// &my_dialog::my_member，this));这允许简单的添加一个对话框
				// 的成员作为小部件的回调，注意大多数小部件可能得到一个回调
				// 如：connect_signal_mouse_left_click(
				// const Signal_function& callback),它隐藏了该函数的平均使用
				template<Ui_event E>
				std::enable_if_t<is_general_event(E)> 
				connect_signal(const Signal_function& signal, 
						const Queue_position position = back_child)
				{
					signal_queue_.connect_signal(E, position, signal);
				}

				// 在set_event中断开回调信号
				template<Ui_event E>
				std::enable_if_t<is_general_event(E)>
				disconnect_signal(const Signal_function& signal,
						const Queue_position position = back_child)
				{
					signal_queue_.disconnect_signal(E, position, signal);
				}

				template<Ui_event E>
				std::enable_if_t<is_mouse_event(E)>
				connect_signal(const Signal_mouse_function& signal,
						const Queue_position position = back_child)
				{
					signal_mouse_queue_.connect_signal(E, position, signal);
				}

				template<Ui_event E>
				std::enable_if_t<is_mouse_event(E)>
				disconnect_signal(const Signal_mouse_function& signal,
						const Queue_position position = back_child)
				{
					signal_mouse_queue_.disconnect_signal(E, position, signal);
				}

				template<Ui_event E>
				std::enable_if_t<is_keyboard_event(E)>
				connect_signal(const Signal_keyboard_function& signal,
						const Queue_position position = back_child)
				{
					signal_keyboard_queue_.connect_signal(E, position, signal);
				}

				template<Ui_event E>
				std::enable_if_t(is_keyboard_event(E))
				disconnect_signal(const Signal_keyboard_function& signal,
						const Queue_position position = back_child)
				{
					signal_keyboard_queue_.disconnect_signal(E, position, signal);
				}

				template<Ui_event E>
				std::enable_if_t(is_touch_event(E))
				connect_signal(const Signal_touch_function& signal, 
						const Queue_position position = back_child)
				{
					signal_touch_queue_.connect_signal(E, position, signal);
				}

				template<Ui_event E>
				std::enable_if_t<is_touch_event(E)>
				disconnect_signal(const Signal_touch_function& signal,
						const Queue_position position = back_child)
				{
					signal_touch_queue_.disconnect_signal(E, position, signal);
				}

				template<Ui_event E>
				std::enable_if_t(is_notification_event(E))
				connect_signal(const Signal_notification_function& signal,
						const Queue_position position = back_child)
				{
					signal_notification_queue_.connect_signal(E, position, signal);
				}

				template<Ui_event E>
				std::enable_if_t(is_notification_event(E))
				disconnect_signal(const Signal_notification_function& signal, const Queue_position position = back_child)
				{
					signal_notification_queue_.disconnect_signal(E, position, signal);
				}

				template<Ui_event E>
				std::enable_if_t<is_message_event(E)>
				connect_signal(const Signal_message_function& signal,
						const Queue_position position = back_child)
				{
					signal_message_queue_.connect_signal(E, position, signal);
				}

				template<Ui_event E>
				std::enable_if_t<is_message_event(E)>
				disconnect_signal(const Signal_message_function& signal,
						const Queue_position position = back_child)
				{
					signal_message_queue_.disconnect_signal(E, position, signal);
				}

				template<Ui_event E>
				std::enable_if_t<is_raw_event(E)>
				connect_signal(const Signal_raw_event_function& signal,
						const Queue_position position = back_child)
				{
					signal_raw_event_queue_.connect_signal(E, position, signal);
				}

				template<Ui_event E>
				std::enable_if_t<is_raw_event(E)>
				disconnect_signal(const Signal_raw_event_function& signal,
						const Queue_position positon = back_child)
				{
					signal_raw_event_queue_.disconnect_signal(E, position, signal);
				}

				template<Ui_event E>
				std::enable_if_t<is_text_input_event(E)>
				connect_signal(const Signal_text_input_function& signal,
						const Queue_position position = back_child)
				{
					signal_text_input_queue_.connect_signal(E, position, signal);
				}

				template<Ui_event E>
				std::enable_if_t<is_text_input_event(E)>
				disconnect_signal(const Signal_text_input_function& signal,
						const Queue_position position = back_child)
				{
					signal_text_input_queue_.disconnect_signal(E, position, signal);
				}

				/*
				 * 鼠标事件行为
				 * 通常，对于鼠标事件，首先检查调度员是否捕获了鼠标，如果它
				 * 发生了事件，如果不是，则从图层的后面到前面搜索调度程序，
				 * 并检查其行为，无事件永远不会发送到图层并进入到下一层，这
				 * 用于可能覆盖按钮的工具提示，但点击工具提示仍然应该点击
				 * 按钮，所有事件总是发送到此图层并停止搜索下一层，点击如果
				 * 鼠标位于调度器区域内，则事件将被发送并进一部搜索，如果
				 * 不再里面测试最后一层。
				 * 如果在这些测试后没有发现调度员则忽略
				 */
				enum Mouse_behavior
				{
					all,
					hit,
					none
				};

				// 捕获鼠标
				void capture_mouse()
				{
					gui::event::capture_mouse(this);
				}

				// 释放鼠标捕获
				void release_mouse()
				{
					gui::event::release_mouse(this);
				}

				//----------------- setters/getters -------------------
				void set_mouse_behavior(const Mouse_behavior mouse_behavior)
				{
					mouse_behavior_ = mouse_behavior;
				}

				Mouse_behavior get_mouse_behavior() const 
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

				// 生成信号类的帮助结构
				template<typename T>
				struct Signal_type
				{
					Signal_type() : pre_child(), child(), post_child() {}

					// 检查给定类型队列是否为空
					bool empty(const Dispatcher::Event_queue_type queue_type) const
					{
						if((queue_type & Dispatcher::pre) && !pre_child.empty())
							return false;
						if((queue_type & Dispatcher::child) && !child.empty())
							return false;
						if((queue_type & Dispatcher::post) && !post_child.empty())
							return false;
						return true;
					}

					std::list<T> pre_child;
					std::list<T> child;
					std::list<T> post_child;
				};

				// 生成各种事件队列的帮助结构
				template<typename T>
				struct Signal_queue
				{
					Signal_queue() {}

					void connect_signal(const Ui_event event,
							const Queue_position position,
							const T& signal)
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
						}
					}

					void disconnect_signal(const Ui_event event,
							const Queue_position position,
							const T& signal)
					{
						Signal_type<T> signal_queue = queue[event];

						/*
						 * 该功能无法区分前后位置，因此从前到后都会fall down
						 * 注意：这仅用于匹配目标类型的第一个信号，这种行文
						 * 未来可能修复
						 */
						switch(position)
						{
							case front_pre_child:
							case back_pre_child:
								signal_queue.pre_child.remove_if(
										[&signal](T& element)
										{ return signal.target_type() == element.target_type(); });
								break;
							case front_child:
							case back_child:
								signal_queue.child.remove_if(
										[&signal](T& element)
										{ return signal.target_type() == element.target_type(); });
								break;
							case front_post_child:
							case back_post_child:
								signal_queue.post_child.remove_if(
										[&signal](T& element)
										{ return signal.target_type() == element.target_type(); });
								break;
						}
					}

					std::map<Ui_event, Signal_type<T>> queue;
				};

				/*
				 * 注册一个热键.
				 * @todo add a static function register_global_hotkey.
				 * 一旦完成，execute_hotkey将首先尝试执行全局热键，如果失败
				 * 则尝试此调度程序中的热键
				 */
				void register_hotkey(const hotkey::Hotkey_command id,
						const Hotkey_function& function);

				// 执行一个热键
				bool execute_hotkey(const hotkey::Hotkey_command id);
			private:
				// 调度程序的鼠标行为
				Mouse_behavior mouse_behavior_;

				/*
				 * 调度员是否想要接受键盘输入
				 * 整个鼠标和键盘处理可以使用代码审查来查看它是否可以组合成
				 * 在一个标志字段中，此时键盘在发送事件前没有看到对话框是否
				 * 有鼠标焦点，所有也许我们应该添加一个活动的调度程序员来跟
				 * 踪它，但由于目前只有非模态窗口和工具提示，所以这不成问题
				 */
				bool want_keyboard_input_;

				// set_event中的回调信号队列
				Signal_queue<Signal_function> signal_queue_;

				// set_event_mouse中的回调信号队列
				Signal_queue<Signal_mouse_function> signal_mouse_queue_;

				// set_event_keyboard中的回调信号队列
				Signal_queue<Signal_keyboard_function> signal_keyboard_queue_;

				// set_event_touch中的回调信号队列
				Signal_queue<Signal_touch_function> signal_touch_queue_;

				// set_event_notification中的回调信号队列
				Signal_queue<Signal_notification_function> signal_notification_queue_;

				// set_event_message中的回调信号队列
				Signal_queue<Signal_message_function> signal_message_queue_;

				// set_raw_event中的回调信号队列
				Signal_queue<Signal_raw_function> signal_raw_event_queue_;

				// set_event_text_input中的回调信号队列
				Signal_queue<Signal_text_input_function> signal_text_input_queue_;

				bool connected_;

				// 在调度程序里注册的热键
				std::map<hotkey::Hotkey_command, Hotkey_function> hotkeys_;
		};

		/*
		 * 可以使用这些helpers轻松的将回调添加到调度程序中，这只是所有其他
		 * 人可以使用的常见列表
		 */

		/*
		 * 连接键盘上的'snooping'信号
		 * 这个回调函数在部件本身之前被调用，允许你监听输入或者过滤它
		 */
		void connect_signal_pre_key_press(Dispatcher& dispatcher,
				const Signal_keyboard_function& signal);

		// 连接信号处理程序以进行鼠标左键单击
		void connect_signal_mouse_left_click(Dispatcher& dispatcher, 
				const Signal_function& signal);

		// 断开鼠标左键单击的信号处理程序
		void disconnect_signal_mouse_left_click(Dispatcher& dispatcher,
				const Signal_function& signal);

		/*
		 * 连接双击鼠标左键的信号处理程序
		 * 我不太确定为什么这种方式可以在切换面板的队列位置上工作，但它确实
		 * 如此，如果它稍后成为问题（即，如果这与其他小部件一起使用并且不起
		 * 作用），将会重新访问。
		 */
		void connect_singal_mouse_left_double_click(Dispatcher& dispatcher,
				const Signal_function& signal);

		// 连接信号处理程序以便在修改时获得通知
		void connect_signal_notify_modified(Dispatcher& dispatcher,
				const Signal_notification_function& signal);
	}
}

#endif
