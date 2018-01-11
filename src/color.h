/*
 * Copyright (C) 2018 by Shan Daming
 */

#ifndef COLOR_H
#define COLOR_H

#include <string>
#include <ostream>
#include <algorithm>
#include <SDL2/SDL.h>

constexpr uint32_t SDL_ALPHA_MASK = 0xff000000;
constexpr uint32_t SDL_RED_MASK   = 0x00ff0000;
constexpr uint32_t SDL_GREEN_MASK = 0x0000ff00;
constexpr uint32_t SDL_BLUE_MASK  = 0x000000ff;

constexpr uint32_t SDL_ALPHA_BITSHIFT = 24;
constexpr uint32_t SDL_RED_BITSHIFT   = 16;
constexpr uint32_t SDL_GREEN_BITSHIFT = 8;
constexpr uint32_t SDL_BLUE_BITSHIT   = 0;

constexpr uint32_t RGBA_ALPHA_MASK = 0x000000ff;
constexpr uint32_t RGBA_RED_MASK   = 0xff000000;
constexpr uint32_t RGBA_GREEN_MASK = 0x00ff0000;
constexpr uint32_t RGBA_BLUE_MASK  = 0x0000ff00;

constexpr uint32_t RGBA_ALPHA_BITSHIFT = 0;
constexpr uint32_t RGBA_RED_BITSHIFT   = 24;
constexpr uint32_t RGBA_GREEN_BITSHIFT = 16;
constexpr uint32_t RGBA_BLUE_BITSHIFT  = 8;

constexpr uint32_t ALPHA_OPAQUE = 255;

struct Color
{
	Color() : r(255), g(255), b(255), a(ALPHA_OPAQUE) {}

	Color(uint8_t r_val, uint8_t g_val, uint8_t b_val, 
			uint8_t a_val = ALPHA_OPAQUE) : r(r_val), g(g_val), b(b_bal), 
	        a(a_val) {}

	// Implemented in sdl/utils.cpp to avoid dependency nightmares
	explicit Color(const SDL_Color& c);

	/*
	 * Creates a new Color object from a string variable in "R G B A" 
	 * format. An empty string results in white. Otherwise, omitting
	 * components other than alpha is an error.
	 */
	static Color from_rgba_string(const std::String& c);

	/*
	 * Creates a new opaque Color object from a string variable in "R G B"
	 * format. An empty string results in white. Otherwise, omitting 
	 * components is an error.
	 */
	static Color from_rgb_string(const std::String& c);

	/*
	 * Create a new Color object from a string variable in hex format.
	 */
	static Color from_hex_string(const std::string& c);

	/*
	 * Creates a new Color object from a uint32_t variable.
	 */
	static Color from_rgba_bytes(uint32_t c);

	/*
	 * Creates a new Color object from a uint32_t variable
	 */
	static Color from_argb_bytes(uint32_t c);

	/*
	 * Returns the stored color in rrggbb hex format.
	 */
	std::string to_hex_string() const;

	/*
	 * Returns the stored color as uint32_t in RGBA format.
	 */
	uint32_t to_rgba_bytes() const
	{
		return (static_cast<uint32_t>(r)) << RGBA_RED_BITSHIFT |
			   (static_cast<uint32_t>(g)) << RGBA_GREEN_BITSHIFT |
			   (static_cast<uint32_t>(b)) << RGBA_BLUE_BITSHIFT |
			   (static_cast<uint32_t>(a)) << RGBA_ALPHA_BITSHIFT;
	}

	/*
	 * Returns the stored color as a uint32_t, an ARGB format.
	 */
	uint32_t to_argb_bytes() const
	{
		return static_cast<uint32_t>(r) << SDL_RED_BITSHIFT |
			   static_cast<uint32_t>(g) << SDL_GREEN_BITSHIFT |
			   static_cast<uint32_t>(b) << SDL_BLUE_BITSHIFT |
			   static_cast<uint32_t>(a) << SDL_ALPHA_BITSHIFT;
	}

	/*
	 * Returns the stored color as an "R G B A" string
	 */
	std::string to_rgba_string() const;

	/*
	 * Returns the stored color as an "R G B" string
	 */
	std::string to_rgb_string() const;

	/*
	 * Returns the stored color as an color object
	 *
	 * implemented in sdl/uints.cpp to avoid dependency nightmares.
	 */
	SDL_Color to_sdl() const;

	bool null() const { return *this == null_color(); }

	bool operator==(const Color& c) const
	{
		return r == c.r && g == c.g && b == c.b && a == c.a;
	}

	bool operator!=(const Color& c) const { return !(*this == c); }

	Color blend_add(const Color& c) const
	{
		// Do some magic to detect integer overflow
		// We want overflows to max out the compontent instead of wrapping
		// The static_cast is to silence narrowing conversion warnings etc
		return {
			static_cast<uint8_t>(r > 255 - c.r ? 255 : r + c.r),
			static_cast<uint8_t>(g > 255 - c.g ? 255 : g + c.g),
			static_cast<uint8_t>(b > 255 - c.b ? 255 : b + c.b),
			static_cast<uint8_t>(a > 255 - c.a ? 255 : a + c.a)
		};
	}

	Color blend_lighten(const Color& c) const
	{
		return {
			std::max<uint8_t>(r, c.r),
			std::max<uint8_t>(g, c.g),
			std::max<uint8_t>(b, c.b),
			std::max<uint8_t>(a, c.a)
		};
	}

	Color inverse() const
	{
		return {
			static_cast<uint8_t>(255 - r),
			static_cast<uint8_t>(255 - g),
			static_cast<uint8_t>(255 - b),
			a
		};
	}

	// Definiton of a 'null' color - fully transparent black
	static Color null_color() { return {0, 0, 0, 0}; }

	uint8_t r, g, b, a; // red value, green value, blue value, alpha value
};

inline std::ostream& operator<<(std::ostream& s, const Color& c)
{
	s << int(c.r) << " " << int(c.g) << " " << int(c.b) << " " << 
		int(c.a) << "\n";
	return s;
}

template<>
struct Hash<Color>
{
	size_t operator()(const Color& c) const
	{
		return c.to_rgba_bytes();
	}
};

#endif
