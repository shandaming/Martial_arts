/*
 * Copyright (C) 2017 by Shan Daming <shandaming@hotmail.com>
 */

#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL2/SDL.h>

class Texture
{
	public:
		Texture() : texture_(nullptr) {}

		Texture(SDL_Texture* t) : texture_(t) {}

		SDL_Texture& create_texture_from_surface(SDL_Renderer* r,
				SDL_Surface* s)
		{
			if(texture_)
				free_texture();
			if(s && r)
			{
				texture_ = SDL_createTextureFromSurface(r, s);
				SDL_FreeSurface(s);
				s = nullptr;
				return *this;
			}
		}

		Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;

		~Texture()
		{
			free_texture();
		}

		operator SDL_Texture*() const
		{
			return texture_;
		}

		SDL_Texture* get() const
		{
			return texture_;
		}

		SDL_Texture* operator->() const
		{
			return texture_;
		}

		bool null() const
		{
			return texture_ == nullptr;
		}
	private:
		void free_texture()
		{
			if(texture_)
				SDL_DestroyTexture(texture_);
		}

		SDL_Texture * texture_;
};

bool operator<(const Texture& a, const Texture& b);

struct Texture_restorer
{
	public:
		Texture_restorer() = default;

		explicit Texture_restorer(class Video* target, const SDL_Rect& rect);

		~Texture_restorer();

		void restore() const;
		void restore(SDL_Rect const& dst) const;

		void update();

		void cancel();

		const SDL_Rect& area() const
		{
			return rect_;
		}
	private:
		class Video* target_;
		SDL_Rect rect_;

		Texture texture_;
};

struct Clip_rect_setter
{
	Clip_rect_setter(const Window& w, const SDL_Rect* r, bool operate = true) : renderer_(w), rect_(), operate_(operate)
	{
		if(operate_)
		{
			SDL_RenderGetClipRect(renderer_, &rect_);
			SDL_RenderSetClipRect(renderer_, r);
		}
	}

	~Clip_rect_setter()
	{
		if(operate_)
			SDL_RenderSetClipRect(renderer_, &rect_);
	}
private:
	SDL_Renderer* renderer_;
	SDL_Rect rect_;
	const bool operate_;
};

#endif
