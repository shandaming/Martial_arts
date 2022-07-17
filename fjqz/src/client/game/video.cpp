#include <SDL2/SDL.h>
#include "video.h"
#include "window.h"
#include "log.h"
#include "errors.h"

video::video() : window_() 
{
	initSDL();
}

void video::initSDL()
{
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
		LOG_FATAL("Could not initialize SDL_video: %s\n", SDL_GetError());
        exit(1);
	}
}

video::~video()
{
	SDL_Quit();
}

void video::video_event_handler::handle_window_event(const SDL_Event& event)
{
	if(event.type == SDL_WINDOWEVENT)
	{
		switch(event.window.event)
		{
			case SDL_WINDOWEVENT_RESIZED:
			case SDL_WINDOWEVENT_RESTORED:
			case SDL_WINDOWEVENT_SHOWN:
			case SDL_WINDOWEVENT_EXPOSED:
				// if(display::get_singleton())
				// display::get_singleton()->redraw_everything();
				SDL_Event drawEvent;
				sdl::UserEvent data(DRAW_ALL_EVENT);

				drawEvent.type = DRAW_ALL_EVENT;
				drawEvent.user = data;

				SDL_FlushEvent(DRAW_ALL_EVENT);
				SDL_PushEvent(&drawEvent);
				break;
		}
	}
}


void video::init_window()
{
	uint32_t window_flags = 0;

#if 0
	window_flags |= SDL_WINDOW_RESIZABLE;

	if(preferences::fullscreen()) {
		window_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	} else if(preferences::maximized()) {
		window_flags |= SDL_WINDOW_MAXIMIZED;
	}
#endif

	window_ = std::make_unique<window>("Fjqz", 1280, 720, window_flags);

	// window_->set_minimum_size(preferences::min_window_width, preferences::min_window_height);

	SDL_DisplayMode currentDisplayMode;
	SDL_GetCurrentDisplayMode(window_->get_display_index(), &currentDisplayMode);
	refresh_rate_ = currentDisplayMode.refresh_rate != 0 ? currentDisplayMode.refresh_rate : 60;

	event_handler_.join_global();
}

SDL_Rect video::screen_area() const
{
	/*
	if(!window_) {
		return {0, 0, frameBuffer->w, frameBuffer->h};
	}
	*/
	ASSERT(window_, "windows_ null potion.");
	// First, get the renderer size in pixels.
	SDL_Point size = window_->get_renderer_output_size();

	return {0, 0, size.x, size.y};
}

int video::get_width() const
{
	return screen_area().w;
}

int video::get_height() const
{
	return screen_area().h;
}

void video::delay(unsigned int milliseconds)
{
	if(1 /*!game_config::no_delay*/)
		SDL_Delay(milliseconds);
}

void video::flip()
{
	if(window_) {
		window_->render();
	}
}

void video::clear_screen()
{
	if(!window_) {
		return;
	}

	window_->fill_color(0, 0, 0, 255);
}

window* video::get_window()
{
	return window_.get();
}


video* video::instance()
{
	static video instance;
	return &instance;
}
