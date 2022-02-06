#include <SDL2/SDL_render.h>

#include "window.h"
#include "errors.h"
#include "point.h"

window::window(const std::string& title, int w, int h,
		uint32_t window_flags) : 
		window_(SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, window_flags))
{
	ASSERT(window_, "Failded to create a SDL_Window object.");

	ASSERT(SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED), 
			"Failed to create a SDL_Renderer object.");

	SDL_RendererInfo info;
	ASSERT(!SDL_GetRendererInfo(*this, &info), 
			"Failed to retrieve the information of the renderer.");

	ASSERT(info.num_texture_formats, "The renderer has no texture information available.");

	SDL_SetRenderDrawBlendMode(*this, SDL_BLENDMODE_BLEND);

	SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, "0");
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

	fill_color(0, 0, 0);
	render();
}

window::~window()
{
	if(window_)
		SDL_DestroyWindow(window_);
}

void window::set_size(int w, int h)
{
	SDL_SetWindowSize(window_, w, h);
}

SDL_Point window::get_size()
{
	SDL_Point size;
	SDL_GetWindowSize(*this, &size.width, &size.height);

	return size;
}

window_size window::get_renderer_output_size()
{
	SDL_Point size;
	SDL_GetRendererOutputSize(*this, &size.width, &size.height);

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

void window::fill_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	SDL_SetRenderDrawColor(*this, r, g, b, a);
	ASSERT(!SDL_RenderClear(*this), "Failed to clear the SDL_Renderer object.");
}

void window::render()
{
	SDL_RenderPresent(*this);
}

void window::set_title(const std::string& title)
{
	SDL_SetWindowTitle(window_, title.c_str());
}

void window::set_icon(SDL_Surface* icon)
{
	SDL_SetWindowIcon(window_, icon);
}

uint32_t window::get_flags()
{
	return SDL_GetWindowFlags(window_);
}

void window::set_minimum_size(window_size& minimum_size)
{
	SDL_SetWindowMinimumSize(window_, minimum_size.width, minimum_size.height);
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
