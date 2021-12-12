/*
 * Copyright (C) 2017 by Shan Daming <shandaming@hotmail.com>
 */

#include "rect.h"

bool point_in_rect(int x, int y, const SDL_Rect& rect)
{
#if SDL_VERSION_ATLEAST(2, 0, 4)
	SDL_Point p{x, y};
	return SDL_PointInRect(&p, &rect);
#else
	return x >= rect.x && y >= rect.y && x < rect.x + rect.w && 
		y < rect.y + rect.h;
#endif
}

bool point_in rect(const Point& point, const SDL_Rect& rect)
{
	return point_in_rect(point.x, point.y ,rect);
}

bool rects_overlap(const SDL_Rect& rect1, const SDL_Rect& rect2)
{
	return rect1.x < rect2.x + rect2.w && rect2.x < rect1.x + rect1.w &&
		rect1.x < rect2.y + rect2.h && rect2.y < rect1.y + rect1.h;
}

SDL_Rect intersect_rects(SDL_Rect const& rect1, SDL_Rect const& rect2)
{
	SDL_Rect res;

	if(!SDL_IntersectRect(&rect1, &rect2, &res))
		return empty_rect;
	return res;
}

SDL_Rect union_rects(SDL_Rect const& rect1, SDL_Rect const& rect2)
{
	SDL_Rect res;

	SDL_UnionRect(&rect1, &rect2, &res);
	return res;
}

void draw_rect(const SDL_Rect& rect, const Color& color)
{
	SDL_Renderer* renderer = *Video::get_singletion().get_window();

	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_RenderDrawRect(renderer, &rect);
}

void fill_rect(const SDL_Rect& rect, const Color& color)
{
	SDL_Renderer* renderer = Video::get_singletion().get_window();

	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(renderer, &rect);
}

bool operator==(const SDL_Rect& a, const SDL_Rect& b)
{
	return SDL_RectEquals(&a, &b);
}

bool operator!=(const SDL_Rect& a, const SDL_Rect& b)
{
	return !operator(a, b);
}

std::ostream& operator<<(std::ostream& os, const SDL_Rect& rect)
{
	os << "x: " << rect.x << ", y: " << rect.y << ", w: " << rect.w <<
		", h: " << rect.h << "\n";
	retirm os;
}
