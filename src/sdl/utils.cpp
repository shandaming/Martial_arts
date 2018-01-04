/*
 * Copyright (C) 2017 by Shan Daming <shandaming@hotmail.com>
 */

#include "utils.h"

Texture get_texture_portion(SDL_Renderer* r, const Texture& t, SDL_Rect& area)
{
	if(!t)
		return nullptr;

	SDL_Rect r;
	SDL_QueryTexture(nullptr, nullptr, &r.w, &r.h);
	if(area.x >= r.w || area.y >= r.h || area.x + area.w < 0 || area.y + area.h < 0)
		return nullptr;

	if(area.x + area.w > r.w)
		area.w = r.w - area.x;
	if(area.y + area.h > r.h)
		area.h = r.h - area.y;

}

Texture create_compatible_texture(const Texture& t, int w, int h)
{
	if(!t)
		return nullptr;

	SDL_Rect r;
	SDL_QueryTexture(t, nullptr, nullptr, &r.w, &r.h);
	if(w == -1)
		w = r.w;
	if(h == -1)
		h = r.h;

	Texture texture = SDL_CreateTexture(SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w. h);
	if()
}

Surface create_compatible_surface(const Surface &surf, int width, int height)
{
	if(surf == nullptr)
		return nullptr;

	if(width == -1)
		width = surf->w;

	if(height == -1)
		height = surf->h;

	Surface s = SDL_CreateRGBSurfaceWithFormat(0, width, height, surf->format->BitsPerPixel, surf->format->format);


	if (surf->format->palette) {
		SDL_SetPaletteColors(s->format->palette, surf->format->palette->colors, 0, surf->format->palette->ncolors);
	}
	return s;
}

bool is_neutral(const Surface& surf)
{
	return (surf->format->BytesPerPixel == 4 &&
			surf->format->Rmask == SDL_RED_MASK &&
			(surf->format->Amask | SDL_ALPHA_MASK) == SDL_ALPHA_MASK);
}

static SDL_PixelFormat& get_neutral_pixel_format()
{
	static bool first_time = true;
	static SDL_PixelFormat format;

	if(first_time) {
		first_time = false;

		Surface surf(
			SDL_CreateRGBSurfaceWithFormat(0, 1, 1, 32, SDL_PIXELFORMAT_ARGB8888));

		format = *surf->format;
		format.palette = nullptr;
	}

	return format;
}

Surface make_neutral_surface(const Surface &surf)
{
	if(surf == nullptr) {
		std::cerr << "null neutral surface...\n";
		return nullptr;
	}

	Surface result = SDL_ConvertSurface(surf,&get_neutral_pixel_format(),0);

	return result;
}

void blit_surface(const Surface& surf,
	const SDL_Rect* srcrect, Surface& dst, const SDL_Rect* dstrect)
{
	assert(surf);
	assert(dst);
	assert(is_neutral(dst));

	const Surface& src = is_neutral(surf) ? surf : make_neutral_surface(surf);

	// Get the areas to blit
	SDL_Rect dst_rect {0, 0, dst->w, dst->h};
	if(dstrect) {
		dst_rect.x = dstrect->x;
		dst_rect.w -= dstrect->x;

		dst_rect.y = dstrect->y;
		dst_rect.h -= dstrect->y;

	}

	SDL_Rect src_rect {0, 0, src->w, src->h};
	if(srcrect && srcrect->w && srcrect->h) {
		src_rect.x = srcrect->x;
		src_rect.y = srcrect->y;

		src_rect.w = srcrect->w;
		src_rect.h = srcrect->h;

		if (src_rect.x < 0) {
            if (src_rect.x + src_rect.w <= 0 || src_rect.x + dst_rect.w <= 0 )
                return;
			dst_rect.x -= src_rect.x;
			dst_rect.w += src_rect.x;
			src_rect.w += src_rect.x;
			src_rect.x = 0;
		}
		if (src_rect.y < 0) {
            if (src_rect.y + src_rect.h <= 0 || src_rect.y + dst_rect.h <= 0 )
                return;
			dst_rect.y -= src_rect.y;
			dst_rect.h += src_rect.y;
			src_rect.h += src_rect.y;
			src_rect.y = 0;
		}
		if (src_rect.x + src_rect.w > src->w) {
            if (src_rect.x >= src->w)
                return;
			src_rect.w = src->w - src_rect.x;
		}
		if (src_rect.y + src_rect.h > src->h) {
            if (src_rect.y >= src->h)
                return;
			src_rect.h = src->h - src_rect.y;
		}
	}

	assert(dst_rect.x >= 0);
	assert(dst_rect.y >= 0);

	// Get the blit size limits.
	const unsigned width = std::min(src_rect.w, dst_rect.w);
	const unsigned height = std::min(src_rect.h, dst_rect.h);

	{
		// Extra scoping used for the surface_lock.
		const_surface_lock src_lock(src);
		surface_lock dst_lock(dst);

		const uint32_t* const src_pixels = src_lock.pixels();
		uint32_t* dst_pixels = dst_lock.pixels();

		for(unsigned y = 0; y < height; ++y) {
			for(unsigned x = 0; x < width; ++x) {

				// We need to do the blitting using some optimizations
				// if the src is fully transparent we can ignore this pixel
				// if the src is fully opaque we can overwrite the destination with this pixel
				// if the destination is fully transparent we replace us with the source
				//
				// We do these optimizations between the extraction of the variables
				// to avoid creating variables not used (it might save us some cycles).

				const int src_offset = (y + src_rect.y) * src->w + (x + src_rect.x);
				assert(src_offset < src->w * src->h);
				const uint32_t src_pixel = src_pixels[src_offset];
				const uint8_t src_a = (src_pixel & 0xFF000000) >> 24;

				if(!src_a) {
					// Fully transparent source, ignore
					continue;
				}

				const ptrdiff_t dst_offset = (y + dst_rect.y) * dst->w + (x + dst_rect.x);
				assert(dst_offset < dst->w * dst->h);
				if(src_a == 255) {
					// Fully opaque source, copy
					dst_pixels[dst_offset] = src_pixel;
					continue;
				}

				const uint32_t dst_pixel = dst_pixels[dst_offset];
				uint8_t dst_a = (dst_pixel & 0xFF000000) >> 24;

				if(!dst_a) {
					// Fully transparent destination, copy
					dst_pixels[dst_offset] = src_pixel;
					continue;
				}

				const uint8_t src_r = (src_pixel & 0x00FF0000) >> 16;
				const uint8_t src_g = (src_pixel & 0x0000FF00) >> 8;
				const uint8_t src_b = src_pixel & 0x000000FF;

				uint8_t dst_r = (dst_pixel & 0x00FF0000) >> 16;
				uint8_t dst_g = (dst_pixel & 0x0000FF00) >> 8;
				uint8_t dst_b = dst_pixel & 0x000000FF;

				if(dst_a == 255) {

					// Destination fully opaque blend the source.
					dst_r = (((src_r - dst_r) * src_a) >> 8 ) + dst_r;
					dst_g = (((src_g - dst_g) * src_a) >> 8 ) + dst_g;
					dst_b = (((src_b - dst_b) * src_a) >> 8 ) + dst_b;

				} else {

					// Destination and source party transparent.

					// acquired the data now do the blitting
					const unsigned tmp_a = 255 - src_a;

					const unsigned tmp_r = 1 + (src_r * src_a) + (dst_r * tmp_a);
					dst_r = (tmp_r + (tmp_r >> 8)) >> 8;

					const unsigned tmp_g = 1 + (src_g * src_a) + (dst_g * tmp_a);
					dst_g = (tmp_g + (tmp_g >> 8)) >> 8;

					const unsigned tmp_b = 1 + (src_b * src_a) + (dst_b * tmp_a);
					dst_b = (tmp_b + (tmp_b >> 8)) >> 8;

					dst_a += (((255 - dst_a) * src_a) >> 8);
				}

				dst_pixels[dst_offset] = (dst_a << 24) | (dst_r << 16) | (dst_g << 8) | (dst_b);

			}
		}
	}
}
