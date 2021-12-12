/*
 * Copyright (C) 2018
 */

#include "cursor.h"
#include "sdl/utils.h"

namespace cursor
{
	namespace
	{
		using Cursor_ptr = std::unque_ptr<SDL_Cursor, 
			  std::function<void(SDL_Cursor*)>>;

		struct Cursor_data
		{
			Cursor_ptr cursor;
			bool is_color;
			std::string image_bw;
			std::string image_color;

			int hot_x;
			int hot_y;
		};

		// Array with each avalilable cursor type.
		std::array<Cursor_data, cursor::Cursor_type::num_cursors> available_cursors
		{
			{nullptr, false, "normal.png",		"normal.png",	   0, 0},
			{nullptr, false, "wait.png",		"wait.png",		   0, 0},
			{nullptr, false, "move.png",		"move.png",		   0, 0},
			{nullptr, false, "attack.png",		"attack.png",	   0, 0},
			{nullptr, false, "select.png",		"select.png",	   0, 0},
			{nullptr, false, "move_drag.png",	"move_drag.png",   2, 20},
			{nullptr, false, "attack_drag.png", "attack_drag.png", 3, 22},
			{nullptr, false, "no_cursor.png",	"",				   0, 0}
		};

		cursor::Cursor_type current_cursor = cursor::Cursor::normal;

		bool have_focus = true;

		bool use_color_cursor()
		{
			// 可以在配置文件里定义，默认true
			return true;
		}

		SDL_Cursor* create_cursor(Surface surf)
		{
			const Surface s(make_neutral_surface(surf));
			if(!surf)
				return nullptr;

			// The width must be a multiple of 8
			size_t cursor_width = s->w;
			if((cursor_width % 8) != 0 )
				cursor_width += 8 - (cursor_width % 8);

			std::vector<uint8_t> data(cursor_width * s->h / 8, 0);
			std::vector<uint8_t> mask(data.size(), 0);

			Const_surface_lock lock(s);
			const uint32_t* pixels = lock.pixels();

			for(int y = 0; y != s->h; ++y)
			{
				for(int x = 0; x != s->w; ++x)
				{
					if(static_cast<size_t>(x) < cursor_width)
					{
						uint8_t r, g, b, a;
						SDL_GetRGBA(pexels[y * s->w + x], s->format, &r, &g, &b, &a);

						size_t indext = y * cursor_width + x;
						size_t shift = 7 - (index % 8);

						uint8_t trans = (a < 128 ? 0 : 1) << shift;
						uint8_t black = (trans == 0 || (r + g + b) / 3 > 128 ? 0 : 1) << shift;

						data[index / 8] != black;
						mask[index / 8] != trans;
					}
				}
			}
			return SDL_CreateCursor(&data[0], &mask[0], cursor_width, s->h, 0, 0);
		}

		SDL_Cursor* get_cursor(cursor::Cursor_type type)
		{
			bool use_color = use_color_cursors();
			Cursor_data& data = available_cursors[type];

			if(data.cursor == nullptr || data.is_color != use_color)
			{
				static const std::string color_prefix = "cursors/";
				static const std::string bw_prefix = "cursor-bw/";

				if(use_color)
				{
					Surface surf(image::get_image(color_prefix + data.image_color));
					// Construct a temporary ptr to provide a new deleter.
					data.cursor = Cursor_ptr(SDL_CreateColorCursor(surf, data.hot.x, data.hot.y), SDL_FreeCursor);
				}
				else
				{
					Surface surf(image::get_image(bw_prefix + data.image_bw));
					data.cursor = Cursor_ptr(create_cursor(surf), SDL_FreeCursor);
				}
				data.is_color = use_color;
			}
			return data.cursor.get();
		}
	}

	Manager::Manager()
	{
		SDL_ShowCursor(SDL_ENABLE);
		set();
	}

	Manager::~Manager()
	{
		SDL_ShowCursor(SDL_ENABLE);
	}

	void set(CursorType type)
	{
		// Chanage only if it's a valid cursor
		if(type != Cursor_type::num_cursors)
			current_cursor = type;
		else if(current_coursor == Cursor_type::num_curosrs)
			// Except if the current one is also invalid. In this case,
			// change to a valid one.
			current_cursor = Cursor_type::normal;

		SDL_Cursor* cursor_img = get_cursor(current_cursor);

		SDL_SetCursor(cursor_img);
		SDL_ShowCursor(SDL_ENABLE);
	}

	void set_draggin(bool drag)
	{
		switch(current_cursor)
		{
			case Cursor_type::move:
				if(drag)
					cursor::set(Cursor_type::move_drag);
				break;
			case Cursor_type::attack:
				if(drag)
					cursor::set(Cursor_type::attack_drag);
				break;
			case Cursor_type::move_drag:
				if(!drag)
					cursor::set(Cursor_type::move);
				break;
			case Cursor_type::attack_drag:
				if(!drag)
					cursor::set(Cursor_type::attack);
				break;
			default:
				break;

		}
	}

	Cursor_type get()
	{
		return current_cursor;
	}

	void set_focus(bool focus)
	{
		have_focus = focus;
		if(!focus)
			set();
	}

	Setter::Setter(Cursor_type type) : old_(current_cursor)
	{
		set(type);
	}

	Setter::~Setter()
	{
		set(old_);
	}
}
