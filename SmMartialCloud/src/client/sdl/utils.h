/*
 * Copyright (C) 2017 by Shan Daming <shandaming@hotmail.com>
 */

#ifndef UTILS_H
#define UTILS_H

#include <SDL2/SDL.h>
#include "../version.h"

Version_info sdl_get_version();

bool is_neutral(const Surface& surf);

Surface make_neutral_surface(const Surface &surf);

inline void sdl_render_copy(SDL_Renderer* r, Texture t, SDL_Rect* src, SDL_Rect* dst)
{
	SDL_RenderCopy(r, t, src, dst);
}

void render_surface(SDL_Surface* s, const SDL_Rect* dest);

inline void sdl_render_copy_portion(SDL_Renderer* r, Texture t, SDL_Rect* src, SDL_Rect* dst)
{
	SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
	SDL_SetTextureBlendMode(t, SDL_BLENDMODE_NONE);

	SDL_RenderCopy(r, t, src, dst);
	
	SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(t, SDL_BLENDMODE_BLEND);
}

Texture get_texture_portion(SDL_Renderer* r, const Texture& t, SDL_Rect& area);

Surface create_compatible_surface(const Surface &surf, int width, int height);

Texture adjust_texture_color(const Texture& t, int r, int g, int b);
Texture adjust_texture_alpha(const Texture& t, Uint8 a);

void blit_surface(const Surface& surf,
	const SDL_Rect* srcrect, Surface& dst, const SDL_Rect* dstrect);

#endif
