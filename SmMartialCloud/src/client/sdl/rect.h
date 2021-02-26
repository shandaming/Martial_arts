/*
 * Copyright (C) 2017 by Shan Daming <shandaming@hotmail.com>
 */

#ifndef RECT_H
#define RECT_H

#include <string>
#include <iostream>
#include <SDL2/SDL.h>
#include "point.h"
#include "../color.h"

constexpr SDL_Rect empty_rect{0, 0, 0, 0};

/*
 * Create an SDL_Rect with the given dimensions.
 */
inline SDL_Rect create_rect(const int x, const int y, const int w, const int h)
{
	return {x, y, w, h};
}

/*
 * Test whether a point is inside a rectangle.
 *
 * @x		The x coordinate of the point.
 * @y		The y coordinate of the point.
 * @rect	The rectagnel.
 *
 * @return	Ture if point(x, y) is inside of on the border of rect, 
 *			false otherwise.
 */
bool point_in_rect(int x, int y, const SDL_Rect& rect);

bool point_in_rect(const Point& point, const SDL_Rect& rect);

/*
 * Test whether two rectangles overlap.
 *
 * @rect1		One rectangle.
 * @rect2		Another rectangle.
 *
 * @return		Ture if rect1 and rect2 intersect, false if not.
 *				Touching borders don't overlap.
 */
bool rects_overlap(const SDL_Rect& rect1, const SDL_Rect& rect2);

/*
 * Calculates the intersection of two rectangles.
 */
SDL_Rect intersect_rects(SDL_Rect const& rect1, SDL_Rect const& rect2);

/*
 * Calculates the union of two rectangles.
 *
 * Note: "union" her doesn't mean the union of the sets of points of the 
 * two polygons, but rather the minimal rectangle that supersets both
 * reangles.
 */
SDL_Rect union_rects(SDL_Rect const& rect1, SDL_Rect const& rect2);

/*
 * Draw a rectangle outline.
 */
void draw_rect(const SDL_Rect& rect, const Color& color);

/*
 * Draw a filled rectangle.
 */
void fill_rect(const SDL_Rect& rect, const Color& color);

/*
 * Fill a rectangle on a given texture. Alias for SDL_RenderFillRect. 
 */
inline void fill_renderer_rect(SDL_Renderer* renderer, const SDL_Rect& rect)
{
	SDL_RenderFillRect(renderer, &rect);
}

bool operator==(const SDL_Rect& rect1, const SDL_Rect& rect2);
bool operator!=(const SDL_Rect& rect1, const SDL_Rect& rect2);

#endif
