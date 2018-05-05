/*
 * Copyright (C) 2017 by Shan Daming <shandaming@hotmail.com>
 */

#include "texture.h"
#include "../video.h"
#include "../log.h"

static lg::Log_domain log_sdl("SDL");
#define ERR_SDL LOG_STREAM(lg::err, log_sdl)

namespace
{
	constexpr int default_texture_format = SDL_PIXELFORMAT_ARGB8888;

	void cleanup_texture(SDL_Texture* t)
	{
		if(t)
			SDL_DestroyTexture(t);
	}
}

Texture::Texture() : texture_(nullptr) {}

Texture::Texture(SDL_Texture* t) : texture_(t, &cleanup_texture)
{
	finalize();
}

Texture::Texture(const Surface& surf) : texture_(nullptr)
{
	SDL_Renderer* r = Video::get_singleton().get_renderer();
	if(!r)
		return;

	texture_.reset(SDL_CreateTextureFromSurface(r, surf), &cleanup_texture);
	if(!texture_)
		ERR_SDL << "When creating texture from surface: " << SDL_GetError()
			<< std::endl;
}

Texture::Texture(int w, int h, SDL_TextureAccess access) : texture_(nullptr)
{
	reset(w, h, access);
}

void Texture::finalize()
{
	set_texture_blend_mode(*this, SDL_BLENDMODE_BLEND);
}

void Texture::reset()
{
	if(texture_)
		texture_.reset();
}

void Texture::reset(int w, int h, SDL_TextureAccess access)
{
	reset();
	SDL_Renderer* r = Video::get_singleton().get_renderer();
	if(!r)
		return;

	texture_.reset(SDL_CreateTexture(r, default_texture_fromat, access, w
				h), &cleanup_texture);
	if(!texture_)
		ERR_SDL << "When creating texture: " << SDL_GetError() << std::endl;

	finalize();
}

void Texture::assign(SDL_Texture* t) 
{
	texture_.reset(t, &cleanup_texture);
}

Texture(Texture&& t) : texture_(std::move(t)) {}

Texture& operator=(Texture&& t)
{
	texture_ = std::move(t);
	return *this;
}

Texture::Info::Info(SDL_Texture* t) : format(0), access(0), w(0), h(0)
{
	SDL_QueryTexture(t, &format, &w, &h);
}
