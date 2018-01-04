/*
 * Copyright (C) 2017 by Shan Daming <shandaming@hotmail.com>
 */

#include "video.h"

Video* Video::singleton_ = nullptr;

Texture frame_buffer = nullptr;
bool fake_interactive = false;

Video::Video(FAKE_TYPES type): window(), fake_screen_(false), 
	help_string_(0), updates_locked_(0), flip_locked_(0)
{
	assert(!singleton_);
	singleton = this;

	init_sdl();

	switch(type)
	{
		case NO_FAKE:
			break;
		case FAKE:
			make_fake();
			break;
		case FAKE_TEST:
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
	singletion_ = nullptr;

	printf("Called SDL_Quit()\n");
}

bool Video::non_interactive() const
{
	return fake_interactive? false : (window == nullptr);
}

SDL_Rect Video::screen_area()
{
	return {0, 0, frame_buffer->w, frame_buffer->h};
}

void Video::blit_texture(int x, int y, Texture t, SDL_Rect* srcrect, 
		SDL_Rect* clip_rect)
{
	Texture& target(get_texture());
	SDL_Rect dst{x, y, 0, 0};

	const Clip_rect_setter clip_setter(target, clip_rect, 
			clip_rect != NULL);
	SDL_RenderCopy(*window.get(), t, &srcrect, &dstrect);
}

void Video::make_fake()
{
	fake_screen = true;
	frame_buffer = SDL_CreateRGBTexture(*window.get(), 
			SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 16, 16);
}

void Video::make_test_fake(unsigned int width, unsigned int height)
{
	frame_buffer = SDL_CreateRGBTexture(*window.get(), 
			SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_TARGET, width, height);
}

void Video::update_frame_buffer()
{
	if(!window)
		return;

	Texture t = SDL_GetRenderTarget(window.get());
	if(!frame_buffer)
		frame_buffer = t;
	else
	{
		Texture old_texture = frame_buffer;
		push_back(old_texture);
		frame_buffer(t);
	}
}

void Video::init_window()
{
}
