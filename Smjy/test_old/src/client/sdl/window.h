/*
 * Copyright (C) 2017 by Shan Daming <shandaming@hotmail.com>
 */

#ifndef SDL2_WINDOW_H
#define SDL2_WINDOW_H

#include <SDL2/SDL_video.h>
#include <string>

class surface;
struct SDL_Renderer;

class window
{
public:
	window(const std::string& title, int pos_x, int pos_y, int width, int height, 
				uint32_t window_flags, uint32_t render_flags);
	~window();

	window(const window&) = delete;
	window operator=(const window&) = delete;

	SDL_Point get_size();
	SDL_Point get_renderer_output_size();
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
	void set_icon(surface& icon);
	void set_minimum_size(int min_w, int min_h);
	void set_size(int widht, int height);

	operator SDL_Window*();
	operator SDL_Renderer*();
private:
	SDL_Window* window_;
	uint32_t pixel_format_;
};

#endif
