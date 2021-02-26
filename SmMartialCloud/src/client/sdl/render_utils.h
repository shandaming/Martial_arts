/*
 * Copyright (C) 2018
 */

#ifndef RENDER_UTILS_H
#define RENDER_UTILS_H

#include <cassert>
#include "rect.h"
#include "texture.h"
#include "../video.h"
#include "../color.h"

/*
 * Sets the renderer output target to the specified texture.
 */
class Render_target_setter
{
	public:
		explicit Render_target_setter(Texture& t) :
			renderer_(Video::get_singleton().get_renderer()),
			last_target_(nullptr)
		{
			if(renderer_)
			{
				// Validate we can rendere to this texture
				assert(t.get_info().access == SDL_TEXTUREACCESS_TARGET);

				last_target_ = SDL_GetRenderTarget(renderer_);
				SDL_SetRenderTarget(renderer_, t);
			}
		}

		~Render_target_setter()
		{
			if(renderer_)
				SDL_SetRenderTarget(renderer_, last_target_);
		}
	private:
		SDL_Renderer* renderer_;
		SDL_Texture* last_target_;
};


using sdl_rect_getter = void (*)(SDL_Renderer*, SDL_Rect*);
using sdl_rect_setter = int (*)(SDL_Renderer*, const SDL_Rect*);

/*
 * Base class for renderer RATT helpers that operate on SDL_Rects
 *
 * @param G     Getter function. Will fetch the current applicable rect.
 *				That will be restored as the applicable state once this 
 *				object is destroyed.
 *
 * @param S		Setter function
 */
template<sdl_rect_getter G, sdl_rect_setter S>
class Render_raii_rect_setter_base
{
	public:
		explicit Render_raii_rect_setter_base(SDL_Rect* rect) :
			operate_(rect != nullptr),
			last_rect_(),
			renderer_(Video::get_singletion().get_renderer())
		{
			if(renderer_ && operate_)
			{
				(*G)(renderer_, &last_rect_);
				(*S)(renderer_, rect);
			}
		}

		~Render_raii_rect_setter_base()
		{
			if(renderer_ && operate_)
			{
				if(last_rect_ != sdl::empty_rect)
					(*S)(renderer_, &last_rect_);
				else
					(*S)(renderer_, nullptr);
			}
		}
	private:
		const bool operate_;
		SDL_Rect last_rect_;
		SDL_Renderer* renderer_;
};

/*
 * Sets the renderer clip rect.
 */
using render_clip_rect_setter = Render_raii_rect_base<
	&SDL_RenderGetClipRect,
	&SDL_RenderSetClipRect>;

/*
 * Sets the renderer viewport rect
 */
using render_viewport_setter = Render_raii_rect_base<
	&SDL_RenderGetViewport,
	&SDL_RenderSetViewport>;

/*
 * Set renderer drawing color
 */
inline void set_draw_color(SDL_Renderer* renderer, uint8_t r, uint8_t g,
		uint8_t b, uint8_t a)
{
	SDL_SetRenderDrawColor(renderer, r, g, b, a);
}

inline void set_draw_color(SDL_Renderer* renderer, const Color& color)
{
	set_draw_color(renderer, color.r, color.g, color.b, color.a);
}

/*
 * TEXTURE SETTERS
 */
inline void set_texture_alpha(Texture& t, uint8_t amount)
{
	SDL_SetTextureAlphaMod(t, amout);
}

inline void set_texture_blend_color(Texture& t, uint8_t r, uint8_t g,
		uint8_t b)
{
	SDL_SetTextureColorMod(t, r, g, b);
}

inline void set_texture_blend_mode(Texture& t, SDL_BlendMode mode)
{
	SDL_SetTextureMode(t, mode);
}

/*
 * Sets the texture scale quality. Note this should be called "before" a 
 * texture is created. since the hint has no effect on existing texture or
 * render ops.
 *
 * @param value		The scaling mode. Use either 'linear' or 'nearest'
 */
inline void set_texture_scale_quality(const std::string& value)
{
	SDL_SetHintWithPriority(SDL_HINT_RENDER_SCALE_QUALITY, value.c_str(),
			SDL_HINT_OVERRIDE);
}

#endif
