/*
 * Copyright (C) 2017 by Shan Daming <shandaming@hotmail.com>
 */

#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include <SDL2/SDL.h>

class Window
{
	public:
		Window(const std::string& title, int x, int y, int w, int h, 
				Uint32 window_flags,Uint32 render_flags);

		~Window();

		Window(const Window&) = delete;
		WIndow operator=(const Window&) = delete;

		void set_size(int w, int h);

		SDL_Point get_size();

		SDL_Point get_output_size();

		void center();

		void maximize();

		void restore();

		void to_window();

		void full_screen();

		void fill(Uint8 r, Uint8 b, Uint8 g, Uint8 a = 0);

		void render();

		void set_title(const std::string& title);

		void set_icon(const SDL_Surface& icon);

		int get_flags();

		void set_minimum_size(int min_w, int min_h);

		int get_display_index();

		operator SDL_Window*();
		operator SDL_Renderer*();
	private:
		SDL_Window* window_;
		Uint32 pixel_format_;
};

#endif
