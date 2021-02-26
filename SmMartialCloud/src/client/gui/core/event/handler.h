/*
 * Copyright (C) 2018
 */

#ifndef HANDLER_H
#define HANDLER_H

namespace gui
{
	class Manager
	{
		Manager();
		~Manager();
	};

	namespace event
	{
		/*
		 * 发送给Dispatcher的事件
		 * 以SDL为前缀的事件-真实的事件。处理程序员会进行小的解码，例如将
		 * 按钮按下事件分割为适当的事件。没有SDL前缀的事件有另一个信号生成，
		 * 例如SDL_MOUSE_MOTION的windows信号处理程序可以生成MOUSE_ENTER,
		 * MOUSE_MOTION和MOUSE_LEAVE事件并将其发送给他的字节点
		 */
		enum Ui_event
		{
			SDL_ACTIVATE,		// 主程序窗口被激活
			DRAW,				// 定期重绘请求
			CLOSE_WINDOW,		// 关闭当前窗口的请求
			SDL_VIDEO_RESIZE,	// SDL调整大小请求，坐标是新的窗口大小

			SDL_MOUSE_MOTION,	// 鼠标移动事件
			MOUSE_ENTER,		// 鼠标进入小部件事件
			MOUSE_MOTION,		// 鼠标移动小部件事件
			MOUSE_LEAVE,		// 鼠标离开小部件事件

			SDL_LEFT_BUTTON_DOWN,	// SDL鼠标左键按下事件
			SDL_LEFT_BUTTON_UP,		// SDL鼠标松开左按键事件
			LEFT_BUTTON_DOWN,		// 鼠标左键按下小部件事件
			LEFT_BUTTON_UP,			// 鼠标松开左健按上的小部件的事件
			LEFT_BUTTON_CLICK,		// 鼠标左键单击小部件事件
			LEFT_BUTTON_DOUBLE_CLICK,	// 鼠标左键双击小部件事件

			SDL_MIDDLE_BUTTON_DOWN,		// SDL鼠标中间按下事件
			SDL_MIDDLE_BUTTON_UP,		// SDL鼠标松开中间按键事件
			MIDDLE_BUTTON_DOWN,			// See LEFT_BUTTON_DOWN
			MIDDLE_BUTTON_UP,			// See LEFT_BUTTON_UP
			MIDDLE_BUTTON_CLICK,		// See LEFT_BUTTON_CLICK
			MIDDLE_BUTTON_DOUBLE_CLICK,	// See LEFT_BUTTON_DOUBLE_CLICK

			SDL_RIGHT_BUTTON_DOWN,		// SDL鼠标右击事件
			SDL_RIGHT_BUTTON_UP,		// SDL松开鼠标右键事件
			RIGHT_BUTTON_DOWN,			// See LEFT_BUTTON_DOWN
			RIGHT_BUTTON_UP,			// See LEFT_BUTTON_UP
			RIGHT_BUTTON_CLICK,			// See LEFT_BUTTON_CLICK
			RIGHT_BUTTON_DOUBLE_CLICK,	// See LEFT_BUTTON_DOUBLE_CLICK

			SDL_WHEEL_LEFT,		// SDL左滚轮事件
			SDL_WHEEL_RIGHT,	// SDL右滚轮事件
			SDL_WHEEL_UP,		// SDL松开滚轮事件
			SDL_WHEEL_DOWN,		// SDL按下滚轮事件
			SDL_KEY_DOWN,		// SDL按键事件
			SDL_TEXT_INPUT,		// SDL文本输入事件
			SDL_TEXT_EDITING,	// SDL文本编辑事件

			NOTIFY_REMOVAL,		// 由小部件发送通知，告知他人他正被销毁

			NOTIFY_MODIFIED,	// 由小部件发送通知，告知他人他的内容或状态
								// 被修改
								// 每个小部件都记录了修改方法，如果没有记录
								// 请求就没有意义

			REQUEST_PLACEMENT,	// 请求放置小部件，这可能出发其他布局组件
								// 的更新

			RECEIVE_KEYBOARD_FOCUS,	// 小部件获得键盘焦点
			LOSE_KEYBOARD_FOCUS,	// 小部件失去键盘焦点

			SHOW_TOOLTIP,		// 请求小部件显示悬停提示
			NOTIFY_REMOVE_TOOLTIP,	// 请求小部件移除悬停提示
			MESSAGE_SHOW_TOOLTIP,	// 请求根据发送的数据现实提示
			SHOW_HELPTIP,			// 请求小部件显示悬停帮助提示
			MESSAGE_SHOW_HELPTIP,	// 请求根据发送的数据显示帮助提示

			SDL_TOUCH_MOTION,
			SDL_TOUCH_UP,
			SDL_TOUCH_DOWN,

			SDL_RAW_EVENT		// 原始SDL事件
		};

		//-------------------
		
		
		// 赞时不写
		
		
		//-------------------

		// 将调度程序连接到事件处理程序上
		void connect_dispatcher(Dispatcher* dispatcher);

		// 断开调度程序与事件处理程序的连接
		void disconnect_dispatcher(Dispatcher* dispatcher);

		// 按Z顺序获取所有事件派发器
		std::vector<Dispatcher*> get_all_dispatchers();

		/*
		 * 初始化鼠标位置
		 * 在窗口布局后，需要更新鼠标位置以测试他是否进入或离开小部件，在
		 * 关闭窗口后还需要发送虚拟鼠标移动
		 */
		void init_mouse_location();

		/*
		 * 捕获鼠标
		 * 调度员可以捕获鼠标，例如，当他按下按钮时，这意味着之后所有的鼠标
		 * 事件都被发送到该小部件
		 */
		void capture_mouse(Dispatcher* dispatcher);

		// 释放捕获的鼠标
		void release_mouse(Dispatcher* dispatcher);

		/*
		 * 捕获键盘
		 * 调度员可以捕获键盘，例如，当按下按钮时，之后的所有键盘事件都被发
		 * 送到该小部件
		 */
		void capture_keyboard(Dispatcher* dispatcher);

		std::ostream& operator<(std::ostream& stream, const Ui_event event);
	}

	/*
	 * 按照打开的顺序跟踪任何类型的打开窗口（模态，非模态或工具提示）。目前
	 * 仅用作is_in_dialog()的帮助器，但可能对将来有帮助。
	 */
	std::vector<Window*> open_window_stack;

	// 从open_window_stack列表里伤处条目，这应该用来代替pop_back
	void remove_from_window_tack(Window* window);

	// 对话框是打开？
	bool is_in_dialog();
}

#endif
