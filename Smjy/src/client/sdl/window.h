/*
 * Copyright (C) 2017 by Shan Daming <shandaming@hotmail.com>
 */

#ifndef SDL2_WINDOW_H
#define SDL2_WINDOW_H

#include <string>

struct window_size
{
	int width;
	int height;
};

struct point;
struct SDL_Color;

class window
{
public:
	window(const std::string& title, point& coordinate, window_size& size, 
				uint32_t window_flags, uint32_t render_flags);

	~window();

	window(const window&) = delete;
	window operator=(const window&) = delete;

	window_size get_size();
	window_size get_renderer_output_size();
	uint32_t get_flags();
	int get_display_index();

	void center_on_screen();
	void maximize();
	void restore();
	void to_window();
	void full_screen();
	void fill_color(SDL_Color& color);
	void render();

	void set_title(const std::string& title);
	void set_icon(Surface& icon);
	void set_minimum_size(window_size& minimum_size);
	void set_size(window_size& size);

	operator SDL_Window*();
	operator SDL_Renderer*();
private:
	SDL_Window* window_;
	uint32_t pixel_format_;
};

#endif
