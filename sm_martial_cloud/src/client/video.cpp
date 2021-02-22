/*
 * Copyright (C) 2017 by Shan Daming <shandaming@hotmail.com>
 */

#include "video.h"

using FT = Video::FAKE_TYPES;
using ME = Video::MODE_EVENT;

Video* Video::singleton_ = nullptr;

Texture frame_buffer = nullptr;
bool fake_interactive = false;

Video::Video(FAKE_TYPES type): window(), fake_screen_(false), 
	help_string_(0), updates_locked_(0), flip_locked_(0)
{
	assert(!singleton_);
	singleton_ = this;

	init_sdl();

	switch(type)
	{
		case FT::NO_FAKE:
			break;
		case FT::FAKE:
			make_fake();
			break;
		case FT::FAKE_TEST:
			make_test_fake();
			break;
	}
}

void init_sdl()
{
	const int res = SDL_InitSubSystem(SDL_INIT_VIDEO);
	if(res < 0)
	{
		printf("Could not initialize SDL_video: %s\n", SDL_GetError());
		return;
	}
}

Video::~Video()
{
	printf("Calling SDL_Quit()\n");

	SDL_Quit();
	assert(singleton_);
	singleton_ = nullptr;

	printf("Called SDL_Quit()\n");
}

bool Video::non_interactive() const
{
	return fake_interactive? false : (window == nullptr);
}

SDL_Rect Video::screen_area()
{
        int w, h;
        SDL_QueryTexture(frame_buffer, nullptr, nullptr, &w, &h);
	return {0, 0, w, h};
}

void Video::blit_texture(int x, int y, Texture& t, SDL_Rect* srcrect, 
		SDL_Rect* clip_rect)
{
	Texture& target(get_texture());
	SDL_Rect dst{x, y, 0, 0};

	const Clip_rect_setter clip_setter(*window.get(), clip_rect, 
			(clip_rect != NULL));
	SDL_RenderCopy(*window.get(), t, srcrect, &dst);
}

void Video::make_fake()
{
	fake_screen_ = true;
	frame_buffer.assign(SDL_CreateTexture(*window.get(), 
			SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 32, 32));
}

void Video::make_test_fake(unsigned int width, unsigned int height)
{
	frame_buffer.assign(SDL_CreateTexture(*window.get(), 
			SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_TARGET, width, height));
}

void Video::update_framebuffer()
{
	if(!window)
		return;

	Texture t = SDL_GetRenderTarget(*window.get());
	if(!frame_buffer)
		frame_buffer.assign(t);
	else
	{
		frame_buffer = t;
	}
}

void Video::init_window()
{
}
