/*
 * Copyright (C) 2017 by Shan Daming <shandaming@hotmail.com>
 */

#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include "surface.h"

class Window
{
	public:
		Window(const std::string& title, int x, int y, int w, int h, 
				uint32_t window_flags, uint32_t render_flags);

		~Window();

		Window(const Window&) = delete;
		Window operator=(const Window&) = delete;

		void set_size(int w, int h);

		SDL_Point get_size();

		SDL_Point get_output_size();

		void center();

		void maximize();

		void restore();

		void to_window();

		void full_screen();

		// Clears the contents of the winodw with a given color
		void fill(uint8_t r, uint8_t b, uint8_t g, uint8_t a = 0);

		void render();

		void set_title(const std::string& title);

		void set_icon(Surface& icon);

		uint32_t get_flags();

		void set_minimum_size(int min_w, int min_h);

		int get_display_index();

		// Gets the renderer info for this window
		const SDL_RendererInfo& get_renderer_info() const
		{
			return info_;
		}

		operator SDL_Window*();
		operator SDL_Renderer*();
	private:
		SDL_Window* window_;
		SDL_RendererInfo info_;
};

#endif
