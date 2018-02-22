/*
 * Copyright (C) 2017 by Shan Daming
 */

#ifndef SURFACE_H
#define SURFACE_H

#include <SDL2/SDL.h>
#include "../video.h"

class Surface
{
	public:
		Surface() :surface_(nullptr) {}

		Surface(SDL_Surface* surf) : surface_(surf) {}

		Surface(const Surface& s) : surface_(s.get()) 
		{
			add_surface_ref(surface_); 
		}

		Surface(Surface&& s) : surface_(s.get())
		{
			s.surface_ = nullptr;
		}

		~Surface()
		{
			free_surface();
		}

		void assign(SDL_Surface* surf)
		{
			assign_surface_internal(surf);
		}

		void assign(const Surface& s)
		{
			assign_surface_internal(s.get());
		}

		Surface& operator=(const Surface& s)
		{
			assign(s);
			return *this;
		}

		Surface& operator=(Surface&& s)
		{
			free_surface();
			surface_ = s.surface_;
			s.surface_ = nullptr;
			return *this;
		}

		// Intended to be used when SDL has already freed the surface.
		void clear_without_free() { surface_ = nullptr; }

		operator SDL_Surface*() const { return surface_; }

		SDL_Surface* get() const { return surface_; }
                SDL_Surface* operator->() { return surface_; }

		bool null() const { return surface_ == nullptr; }
	private:
		static void add_surface_ref(SDL_Surface* surf)
		{
			if(surf)
				++surf->refcount;
		}

		void assign_surface_internal(SDL_Surface* surf)
		{
			add_surface_ref(surf);
			free_surface();
			surface_ = surf;
		}

		void free_surface();

		SDL_Surface* surface_;
};

bool operator<(const Surface& a, const Surface& b) 
{ 
	return a.get() < b.get(); 
}
/*
struct Surface_restorer
{
	public:
		Surface_restorer();
		Surface_restorer(Video* target_, const SDL_Rect& rect);
		~Surface_restorer();

		void restore() const;
		void restore(SDL_Rect const& dst) const;
		void update();
		void cancel();

		const SDL_Rect& arear() const { return rect_; }
	private:
		Video* target_;
		SDL_Rect rect_;
		Surface surface_;
};

template<typename T>
class Surface_locker
{
	public:
		Surface_locker(T& surf): surface_(surf), locked_(false)
		{
			if(SDL_MUSTLOCK(surface_))
				locked_ = SDL_LockSurface(surface_) == 0;
		}

		~Surface_locker()
		{
			if(locked_)
				SDL_UnlockSurface(surface_);
		}

		pixel_t* pixels() const 
		{
			return reinterpret_cast<pixel_t*>(surface_->pixels);
		}
	private:
		T& surface_;
		bool locked_;
};

using surface_lock = Surface_locker<Surface>;
using const_surface_lock = Surface_locker<const Surface>;

class Clip_rect_setter
{
	public:
		Clip_rect_setter(const Surface& surf, const SDL_Rect* r, 
				bool operate = true) : surface_(surf), rect_(),
				operate_(operate)
		{
			if(operate_)
			{
				SDL_GetClipRect(surface_, &rect_);
				SDL_SetClipRect(surface_, r);
			}
		}

		~Clip_rect_setter()
		{
			if(operate_)
				SDL_SetClipRect(surface_, rect_);
		}
	private:
		Surface surface_;
		SDL_Rect rect_;
		bool operate_;
};
*/
#endif
