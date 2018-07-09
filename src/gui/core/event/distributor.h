/*
 * Copyright (C) 2018
 */

#ifndef DISTRIBUTOR_H
#define DISTRIBUTOR_H

#include "gui/core/event/dispatcher.h"
#include "gui/core/event/handler.h"
#include "sdl/point.h"
#include "video.h"

/*
 * 包含事件分发者
 * 事件发布者存在几个类，他们被合并到一个模板化的分发者类中，这些类紧密结合
 * 在一起。
 * 所有类都可以直接访问每个其他成员，因为他们应该像人一样。（由于按钮是模板
 * 化的子类，因此不可能使用似有子类）。
 * mouse_motion类处理鼠标移动并拥有我们的所有者，因此所有类实际上都继承自我
 * 们。
 * mouse_button类是每个鼠标按钮的模板化类，模板参数用于区分鼠标按钮，虽然很
 * 容易在代码中添加更多的鼠标按钮，但几个地方只能使用左、中、右按钮。
 * 分配器是在用户代码中使用的主要类，这个类还包含键盘的处理。
 */

namespace gui
{
	namespace event
	{
		class Mouse_motion
		{
			public:
				Mouse_motion(Widget& owner, const dispatcher::Queue_position queue_position);
				~Mouse_motion();

				/*
				 * 捕获鼠标输入
				 * 当捕获具有鼠标焦点的小部件时进行捕获
				 */
				void capture_mouse(const bool capture = true);
			protected:
				/*
				 * 启动悬停计时器
				 *
				 * @param widget		需要工具提示的小部件
				 * @param coordinate	锚点坐标
				 */
				void start_hover_timer(Widget* widget, const Point& coordinate);

				// 停止当前悬停计时器
				void stop_hover_timer();

				/*
				 * 当鼠标进入小部件内时调用
				 *
				 * param mouse_over		应该接受事件的小部件
				 */
				void mouse_enter(Widget* mouse_over);

				// 当鼠标离开当前表部件时调用
				void mouse_leave();

				// 当前拥有鼠标焦点的小部件
				Widget* mouse_focus_;

				// 当前小部件捕获了焦点？
				bool mouse_captured_;

				// 拥有的小部件
				Widget& owner_;

				// 悬停事件计时器
				size_t hover_timer_;

				// 得到的悬停事件的小部件
				Widget* hover_widget_;

				// 悬停事件的锚点
				Point hover_position_;

				/*
				 * 该部件是否现实悬停
				 *
				 * 工具提示被触发后，小部件不会获得第二个悬停事件，仅在（很
				 * 快）进入另一个小部件之后，它将再次现实这个小部件
				 */
				bool hover_shown_;
			private:
				/*
				 * 当鼠标移过小部件时调用
				 *
				 * @param mouse_over		应该接收事件的小部件
				 * @param coordinate		当前屏幕鼠标坐标
				 */
				void mouse_hover(Widget* mouse_over, const Point& coordinate);

				// 当鼠标想要小部件现实其工具提示时调用
				void show_tooltip();

				void signal_handler_sdl_mouse_motion(const event::Ui_event event, bool& handled, const Point& coordinate);

				void signal_handler_sdl_wheel(const event::Ui_event event,
						bool& handled, const Point& coordinate);

				void signal_handler_show_helptip(const event::Ui_event event, bool& handled, const Point& coordinate);

				bool signal_handler_sdl_mouse_motion_entered_;
		};

		/*
		 * 小型助手metastruct专用鼠标按钮，并提供UI事件类型别名，而无需制作
		 * 鼠标按钮需要一百万个模板类型
		 */
		template<Ui_event sdl_button_down, Ui_event sdl_button_up,
			Ui_event button_down, Ui_event button_up, Ui_event button_click,
			Ui_event button_double_click>
		struct Mouse_button_event_type_wrapper
		{
			static const Ui_event sdl_button_down_event = sdl_button_down;
			static const Ui_event sdl_button_up_event = sdl_button_up;
			static const Ui_event button_down_event = button_down;
			static const Ui_event button_up_event = button_up;
			static const Ui_event button_click_event = button_click;
			static const Ui_event button_double_click_event = button_double_click;
		};

		template<typename T>
		class Mouse_button : public virtual Mouse_motion
		{
			public:
				Mouse_button(const std::string& name, Widget& owner,
						const dispatcher::Queue_position queue_position);

				/*
				 * 初始化按钮状态
				 *
				 * @param is_down		按钮初始化状态，如果true dwon，否则
				 *						作为up初始化
				 */
				void initialize_state(const bool is_down);
			protected:
				// 用于双击的最后一次点击时间
				uint32_t last_click_stamp_;

				// 最后一次点击的小部件已打开，用于双击
				Widget* last_clicked_widget_;

				/*
				 * 如果没有捕捉到鼠标，我们需要验证它是否在同一个窗口小部件
				 * 上，因此我们发送一个正确的点击，也需要发送到正确的窗口小
				 * 部件。
				 */
				Widget* focus_;
			private:
				void signal_handler_sdl_button_down(const event::Ui_event event, bool& handled, const Point& coordinate);

				void signal_hander_sdl_button_up(const event::Ui_event event, bool& handled, const Point& coordinate);

				void mouse_button_click(Widget* widget);

				// 被用于debug消息
				const std::string& name_;

				// 按钮按下了？
				bool is_down_;

				bool signal_handler_sdl_button_down_entered_;

				bool signal_handler_sdl_button_up_entered_;
		};

		// distributor

		using Mouse_button_left = Mouse_button<
			mouse_button_event_types_wrapper<
			SDL_LEFT_BUTTON_DOWN,
			SDL_LEFT_BUTTON_UP,
			LEFT_BUTTON_DOWN,
			LEFT_BUTTON_UP,
			LEFT_BUTTON_CLICK,
			LEFT_BUTTON_DOUBLE_CLICK>
				>;

		using Mouse_button_middle = Mouse_button<
			mouse_button_event_types_wrapper<
			SDL_MIDDLE_BUTTON_DOWN,
			SDL_MIDDLE_BUTTON_UP,
			MIDDLE_BUTTON_DOWN,
			MIDDLE_BUTTON_UP,
			MIDDLE_BUTTON_CLICK,
			MIDDLE_BUTTON_DOUBLE_CLICK>
				>;

		using Mouse_button_right = Mouse_button<
			mouse_button_event_types_wrapper<
			SDL_RIGHT_BUTTON_DOWN,
			SDL_RIGHT_BUTTON_UP,
			RIGHT_BUTTON_DOWN,
			RIGHT_BUTTON_UP,
			RIGHT_BUTTON_CLICK,
			RIGHT_BUTTON_DOUBLE_CLICK>
				>;

		// 小部件库的事件处理程序类
		class Distributor : public Mouse_button_left, 
							public Mouse_button_middle,
							public Mouse_button_right
		{
			public:
				Distributor(Widget& owner, const dispatcher::Queue_position queue_position);
				
				~Distributor();

				/*
				 * 初始化键盘鼠标的状态
				 *
				 * 需要在初始化和重新激活后
				 */
				void initialize_state();

				/*
				 * 捕获键盘输入
				 *
				 * @param widget		应该捕获键盘的小部件，发送nullptr
				 *						释放捕获
				 */
				void keyboard_capture(Widget* widget);

				/*
				 * 添加小部件到键盘链
				 *
				 * @param widget		要添加到链中的小部件，该小部件时有效
				 *						小部件，尚未添加到链中。
				 */
				void keyboard_add_to_chain(Widget* widget);

				// Remove the widget from the keyboard chain.
				void keyboard_remove_from_chain(Widget* widget);

				// Return the widget currently capturing keyboard in
				Widget* keyboard_focus() const;
			private:
				// 保存键盘焦点的小部件
				Widget* keyboard_focus_;

				/*
				 * 回退键盘焦点项目
				 *
				 * 当关注的小部件没有处理键盘事件(或没有处理键盘焦点)时，它
				 * 将发送此向量中的所有小部件，该命令从rbegin()到rend()，如
				 * 果keyboard_focus_在向量中，它将不会获得2次事件，添加到向
				 * 量的第一项应该是窗口，所以它将成为最后一个处理程序，并可
				 * 以分配注册的热键。
				 */
				std::vector<Widget*> keyboard_focus_chain_;

				// 处理某些事件并将他们发送到适当的小部件的函数集 ，这些函数
				// 由SDL事件处理函数调用
				
				void signal_handler_sdl_key_down(const SDL_Keycode key,
						const SDL_Keymod modifier, 
						const std::string& unicode);

				void signal_handler_sdl_text_input(const std::string& unicode, int32_t start, int32_t len);
				void signal_handler_sdl_text_editiong(const std::string& unicode, int32_t start, int32_t len);

				template<typename Fcn, typename P1, typename P2, 
					typename P3>
				void signal_handler_keyboard_internal(event::Ui_event evt, 
						P1&& p1, P2&& p2, P3&& p3);

				void signal_handler_notify_removal(Dispatcher& widget,
						const Ui_event event);
		};
	}
}

#endif
