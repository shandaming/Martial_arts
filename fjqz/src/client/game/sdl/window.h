/*
 * Copyright (C) 2017 by Shan Daming <shandaming@hotmail.com>
 */

#ifndef SDL2_WINDOW_H
#define SDL2_WINDOW_H

#include <SDL2/SDL_video.h>
#include <string>

struct SDL_Color;
struct SDL_Surface;

class window
{
public:
	window(const std::string& title, int w, int h, 
				uint32_t window_flags);

	~window();

	window(const window&) = delete;
	window operator=(const window&) = delete;

	SDL_Point get_size();
	SDL_Point get_renderer_output_size();
	uint32_t get_flags();
	int get_display_index();

	void center();
	void maximize();
	void restore();
	void to_window();
	void full_screen();
	void fill_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
	void render();

	void set_title(const std::string& title);
	void set_icon(SDL_Surface* icon);
	void set_minimum_size(int width, int height);
	void set_size(int w, int h);

	operator SDL_Window*();
	operator SDL_Renderer*();
private:
	SDL_Window* window_;
};

#endif
