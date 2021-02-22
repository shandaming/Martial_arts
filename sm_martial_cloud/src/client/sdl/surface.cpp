/*
 * Copyright (C) 2017 by Shan Daming
 */

#include "surface.h"

void Surface::free_surface()
{
	if(surface_)
	{
		if(surface_->refcount > 1)
			--surface_->refcount;
		else
			SDL_FreeSurface(surface_);
	}
}
/*
Surface_restorer::Surface_restorer() : target_(nullptr), rect_(empty_rect),
	surface_(nullptr) {}

Surface_restorer::Surface_restorer(Video* target, const SDL_Rect& rect) :
	target_(target), rect_(rect), surface_(nullptr)
{
	update();
}

Surface_restorer::~Surface_restorer()
{
	restore();
}

void Surface_restorer::restore(SDL_Rect const& dst) const
{
	if(surface_.null())
		return;

	SDL_Rect dst2 = intersect_rects(dst, rect_);
	if(dst2.w == 0 || dst2.h == 0)
		return;

	SDL_Rect src = dst2;
	src.x -= rect_.x;
	src.y -= rect_.y;
	sdl
}
*/
