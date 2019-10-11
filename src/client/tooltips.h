/*
 * Copyright (C) 2017 by Shan Daming
 */

#ifndef TOOLTIPS_H
#define TOOLTIPS_H

#include <map>
#include "sdl/rect.h"
#include "sdl/surface.h"

struct Manager
{
	Manager();
	~Manager();
};

void clear_tooltips();

void clear_tooltips(const SDL_Rect& rect);

int add_tooltip(const SDL_Rect& rect, const std::string& message,
		const std::string& action = "", bool use_markup = true,
		const Surface& foreground = Surface(nullptr));

bool update_tooltip(int id, const SDL_Rect& rect, 
		const std::string& message, const std::string& action,
		bool use_markup, const Surface& foreground);
bool update_tooltip(int id, const SDL_Rect& rect,
		const std::string& message, const std::string& action,
		bool use_markup);

void remove_tooltip(int id);

void process(int mousex, int mousey);

/*
 * Check if we clicked on a tooltip having an action. If it is,
 * then execute the action and return true (only possible action
 * are opening help page for the moment)
 */
bool click(int mousex, int mousey);

#endif
