/*
 * Copyright (C) 2018
 */

#ifndef MESSAGE_H
#define MESSAGE_H

#include <SDL2/SDL_rect.h>

/*
 * 该文件包含@ref gui:event::message类的定义
 * 该类用于@ref gui::event::Signal_message_function
 */

namespace gui
{
	namespace event
	{
		/*
		 * 消息回调持有对消息的引用
		 * 消息的内同因类型而异，这个类是一个虚拟析构函数的基础，它可以在接
		 * 收到的类上使用dynamic_cast,以确保发送正确的消息类型。
		 * 这意为着事件中使用的所有消息都需要从这个类派生，当消息不需要“内容
		 * ”时，它可以发送这个类作为消息，通过: @ref REQUEST_PLACEMENT
		 */
		struct Message
		{
			Message() = default;
			// 不允许复制，因为构建副本会丢失确切的类型
			Message(const Message&) = delete;
			virtual ~Message() {}
		};

		// The Message for MESSAGE_SHOW_TOOLTIP
		struct Message_show_tooltip : public Message
		{
			Message_show_tooltip(const std::string& message_, 
					const Point& location_, const SDL_Rect& source_rect_) :
				message(message_), location(location_), 
				source_rect(source_rect_) {}

			// The message to show on the tooltip
			const std::string message;
			// The location where to show the tooltip
			const Point location;
			// The size of the entity requesting to show a tooltip
			const SDL_Rect source_rect;
		};

		// The message for MESSAGE_SHOW_HELPTIP
		struct Message_show_helptip : Message
		{
			Message_show_helptip(const std::string& message_, 
					const Point& location_, const SDL_Rect& source_rect_):
				message(message_), location(location_), 
				source_rect(source_rect_) {}
			// The message to show on the helptip
			const std::string message;
			// The location where to show the helptip
			const Point location;
			// The size of the entity requesting to show a helptip
			const SDL_Rect source_rect_;
		};
	}
}

#endif
