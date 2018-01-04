/*
 * Copyright (C) 2017 by Shan Daming
 */

#include "tooltips.h"

struct Tooltip
{
	Tooltip(const SDL_Rect& r, const std::string& msg, 
			const std::string& act = "", bool use_markup = false,
			const Surface& fg = surface()) : rect(r), message(msg),
			action(act), markup(use_markup), foreground(fg) {}

	SDL_Rect rect;
	std::string message;
	std::string action;
	bool markup;
	Surface foreground;
};

std::map<int, tooltip> tips;
std::map<int, tooltips>::const_iterator current_tooltip = tips.end();

int tooltip_handle = 0;
int tooltip_id = 0;

Surface current_background = nullptr;


int add_tooltip(const SDL_Rect& rect, const std::string& message, 
		const std::string& action, bool use_markup, 
		const Surface& foreground)
{
	for(auto it = tips.begin(); it != tips.end();)
	{
		if(rects_overlap(it->second.rect, rect))
			tips.erase(it++);
		else
			++it;
	}

	int id = tooltip_id++;
	tips.emplace(id, Tooltip(rect, message, action, use_markup, 
				foreground));

	current_tooltip = tips.end();
	return id;
}
