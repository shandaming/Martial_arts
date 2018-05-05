/*
 * Copyright (C) 2017-2018 by Shan Daming <shandaming@hotmail.com>
 */

#ifndef TEXTURE_H
#define TEXTURE_H

#include <memory>
#include <SDL2/SDL.h>
#include "surface.h"

class Texture
{
	public:
		Texture();
		Texture(int w, int h, SDL_TextureAccess access);
		explicit Texture(SDL_Texture* t);
		explicit Texture(const Surface& surf);

		Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;

		Texture(Texture&& t);
		Texture& operator=(Texture&& t);

		struct Info
		{
			explicit Info(SDL_Texture* t);

			Uint32 format;
			int access;
			int w;
			int h;
		};

		const Info get_info() const { return Info(*this); }

		void reset();
		void reset(int w, int h, SDL_TextureAccess access);

		void assign(SDL_Texture* t);

		operator SDL_Texture*() const
		{
			return texture_.get();
		}

		bool null() const
		{
			return texture_ == nullptr;
		}
	private:
		void finalize();

		std::shared_ptr<SDL_Texture> texture_;
};

#endif
