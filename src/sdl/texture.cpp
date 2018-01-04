/*
 * Copyright (C) 2017 by Shan Daming <shandaming@hotmail.com>
 */

#include "texture.h"

Texture_restorer::Texture_restorer() : target_(nullptr), rect_(empty_rect), texture_(nullptr) {}

Texture_restorer::Texture_restorer(Video* target, const SDL_Rect& rect) : target_(target), rect_(rect), texture_(nullptr)
{
	update();
}

Texture_restorer::~Texture_restorer()
{
	restore();
}

void Texture_restorer::restore(SDL_Renderer* r, SDL_Rect const& dst) const
{
	if(texture_.null())
		return;

	SDL_Rect dst2 = intersect_rects(dst, rect_);
	if(dst2.w == 0 || dst2.h == 0)
		return;

	SDL_Rect src = dst2;
	src.x = rect_.x;
	src.y = rect_.y;

	sdl_render_copy(r, target_->get_texture(), &src, &dst2);
}

void Texture_restorer::restore(SDL_Renderer* r) const
{
	if(texture_.null())
		return;

	SDL_Rect dst = rect_;

	sdl_render_copy(r, target_->get_texture(), nullptr,&dst);
}

void Texture_restorer::update()
{
	if(rect_.w <= 0 || rect_.h <= 0)
		texture_.free_texture();
	else
		texture_.assign(get_texture_portion(target_->get_texture(), rect_));
}

void Texture_restorer::cancel()
{
	texture_.assgin(nullptr);
}

bool operator<(const Texture& a, const Texture& b)
{
	return a.get() < b.get();
}
