/*
 * Copyright (C) 2017 by Shan Daming <shandaming@hotmail.com>
 */

#include "window.h"
#include "exception.h"
#include "renderer_utils.h"

Window::Window(const std::string& title, int x, int y, int w, int h,
		uint32_t window_flags, uint32_t render_flags) : 
		window_(SDL_CreateWindow(title.c_str(), x, y, w, h, window_flags)),
		info_()
{
	if(!window_)
		throw Exception("Failed to create a SDL_Window object.", true);

	if(!SDL_CreateRenderer(window_, -1, render_flags))
		throw Exception("Failed to create a SDL_Renderer object.", true);

	if(SDL_GetRendererInfo(*this, &info) != 0)
		throw Exception("Failed to retrieve the information of the \
				renderer.", true);

	if(info.num_texture_formats == 0)
		throw Exception("The renderer has no texture information \
				available.\n", false);

	if(!(info_.flags & SDL_RENDERER_TARGETTEXTURE))
		throw Exception("Render-to-texture not supported or enabled!", 
				false);

	SDL_SetRenderDrawBlendMode(*this, SDL_BLENDMODE_BLEND);
	
	// In fullscreen mode, do not minimize on focus loss.
	// Minimizing was reported as bug #1609 with blocker priority.
	SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, "0");

	fill(0, 0, 0);
	render();
}

Window::~Window()
{
	if(window_)
		SDL_DestroyWindow(window_);
}

void Window::set_size(int w, int h)
{
	SDL_SetWindowSize(window_, w, h);
}

SDL_Point Window::get_size()
{
	SDL_Point res;
	SDL_GetWindowSize(*this, &res.x, &res.y);

	return res;
}

SDL_Point Window::get_output_size()
{
	SDL_Point res;
	SDL_GetRendererOutputSize(*this, &res.x, &res.y);

	return res;
}

void Window::center()
{
	SDL_SetWindowPosition(window_, SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED);
}

void Window::maximize()
{
	SDL_MaximizeWindow(window_);
}

void Window::to_window()
{
	SDL_SetWindowFullscreen(window_, 0);
}

void Window::restore()
{
	SDL_RestoreWindow(window_);
}

void Window::full_screen()
{
	SDL_SetWindowFullscreen(window_, SDL_WINDOW_FULLSCREEN_DESKTOP);
}

void Window::fill(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	set_draw_color(*this, r, g, b, a);
	if(SDL_RenderClear(*this) != 0)
		throw Exception("Failed to clear the SDL_Renderer object.", true);
}

void Window::render()
{
	SDL_RenderPresent(*this);
}

void Window::set_title(const std::string& title)
{
	SDL_SetWindowTitle(window_, title.c_str());
}

void Window::set_icon(Surface& icon)
{
	SDL_SetWindowIcon(window_, icon);
}

uint32_t Window::get_flags()
{
	return SDL_GetWindowFlags(window_);
}

void Window::set_minimum_size(int min_w, int min_h)
{
	SDL_SetWindowMinimumSize(window_, min_w, min_h);
}

int Window::get_display_index()
{
	return SDL_GetWindowDisplayIndex(window_);
}

Window::operator SDL_Window*()
{
	return window_;
}

Window::operator SDL_Renderer*()
{
	return SDL_GetRenderer(window_);
}
