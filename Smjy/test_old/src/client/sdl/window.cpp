/*
 * Copyright (C) 2017 by Shan Daming <shandaming@hotmail.com>
 */

#include <SDL2/SDL_pixels.h>

#include "window.h"
#include "errors.h"

window::window(const std::string& title, int pos_x, int pos_y, int width, int height,
		uint32_t window_flags, uint32_t render_flags) : 
		window_(SDL_CreateWindow(title.c_str(), pos_x, pos_y, width, height, window_flags)),
		pixel_format_(SDL_PIXELFORMAT_UNKNOWN)
{
	if(!window_)
		FATAL("Failed to create a SDL_Window object.");

#if SDL_VERSION_ATLEAST(2, 0, 10)
	SDL_SetHint(SDL_HINT_RENDER_BATCHING, "0"); // 故事模式需要关闭渲染批处理
#endif

	if(!SDL_CreateRenderer(window_, -1, render_flags))
		FATAL("Failed to create a SDL_Renderer object.");

	SDL_RendererInfo info;
	if(SDL_GetRendererInfo(*this, &info) != 0)
		FATAL("Failed to retrieve the information of the renderer.");

	if(info.num_texture_formats == 0)
		FATAL("The renderer has no texture information available.");

	SDL_SetRenderDrawBlendMode(*this, SDL_BLENDMODE_BLEND);

	SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, "0");

	pixel_format_ = info.texture_formats[0];

	fill_color(0, 0, 0);
	render();
}

window::~window()
{
	if(window_)
		SDL_DestroyWindow(window_);
}

void window::set_size(int width, int height)
{
	SDL_SetWindowSize(window_, width, height);
}

SDL_Point window::get_size()
{
	SDL_Point size;
	SDL_GetWindowSize(*this, &size.x, &size.y);

	return size;
}

SDL_Point window::get_renderer_output_size()
{
	SDL_Point size;
	SDL_GetRendererOutputSize(*this, &size.x, &size.y);

	return size;
}

void window::center_on_screen()
{
	SDL_SetWindowPosition(window_, SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED);
}

void window::maximize()
{
	SDL_MaximizeWindow(window_);
}

void window::restore()
{
	SDL_RestoreWindow(window_);
}

void window::to_window()
{
	SDL_SetWindowFullscreen(window_, 0);
}

void window::full_screen()
{
	SDL_SetWindowFullscreen(window_, SDL_WINDOW_FULLSCREEN_DESKTOP);
}

void window::fill_color(SDL_Color& color)
{
	set_draw_color(*this, color.r, color.g, color.b, color.a);
	if(SDL_RenderClear(*this) != 0)
		FALT("Failed to clear the SDL_Renderer object.");
}

void window::render()
{
	SDL_RenderPresent(*this);
}

void window::set_title(const std::string& title)
{
	SDL_SetWindowTitle(window_, title.c_str());
}

void window::set_icon(surface& icon)
{
	SDL_SetWindowIcon(window_, icon);
}

uint32_t window::get_flags()
{
	return SDL_GetWindowFlags(window_);
}

void window::set_minimum_size(int min_w, int min_h)
{
	SDL_SetWindowMinimumSize(window_, min_w, min_h);
}

int window::get_display_index()
{
	return SDL_GetWindowDisplayIndex(window_);
}

window::operator SDL_Window*()
{
	return window_;
}

window::operator SDL_Renderer*()
{
	return SDL_GetRenderer(window_);
}
