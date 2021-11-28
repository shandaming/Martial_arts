/*
 * Copyright (C) 2018
 */

#ifndef KEY_H
#define KEY_H

#include <SDL2/SDL.h>

/*
 * 跟踪键盘上所有按键的类，通过使用其操作[]可以找到任何按键是否被按下。请
 * 注意，通常使用按键事件来查看按键的时间而不是使用此对象进行轮询会更好
 */
class key
{
	public:
		Key();
		bool operator[](int k) const;
		static bool is_uncomposable(const SDL_KeyboardEvent& event);
	private:
		const uint8_t* key_list_;
};

#endif
