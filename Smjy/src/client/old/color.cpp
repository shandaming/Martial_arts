/*
 * Copyright (C) 2018 by Shan Daming
 */

#include <sstream>
#include "color.h"
#include "serialization/string_utils.h"

Color Color::from_rgba_string(const std::string& c)
{
	if(c.empty())
		return null_color();

	std::vector<std::string> fields = utils::split(c);

	// Allow either 3 (automatic opaque alpha) or 4 (explicit alpha) fields
	if(fields.size() != 3 && fields.size() != 4)
		throw std::invalid_argument(
				"Wrong number of components for RGBA color");

	return {
		static_cast<uint8_t>(std::stoul(fields[0])),
		static_cast<uint8_t>(std::stoul(fields[1])),
		static_cast<uint8_t>(std::stoul(fields[2])),
		static_cast<uint8_t>(fields.size() == 4 ? std::stoul(fields[3]) :
				ALPHA_OPAQUE)
	};
}

Color Color::from_rgb_string(const std::string& c)
{
	if(c.empty())
		return null_color();

	std::vector<std::string> fields = utils::split(c);

	if(fields.size() != 3)
		throw std::invalid_argument(
				"Wrong number of components for RGB color");

	return {
		static_cast<uint8_t>(std::stoul(fields[0])),
		static_cast<uint8_t>(std::stoul(fields[1])),
		static_cast<uint8_t>(std::stoul(fields[2])),
		static_cast<uint8_t>(ALPHA_OPAQUE)
	};
}

Color Color::from_hex_string(const std::string& c)
{
	if(c.size() != 6)
		throw std::inalid_argument(
				"Color hex string should be exactly 6 digits");

	unsigned long tmp_c = stoul(c.c_str(), nullptr, 16);

	return {
		static_cast<uint8_t>((0x00ffffff & tmp_c) >> 16),
		static_cast<uint8_t>((0x00ffffff & tmp_c) >> 8),
		static_cast<uint8_t>(0x00ffffff & tmp_c),
		ALPHA_OPAQUE
	};
}

Color Color::from_rgba_bytes(uint32_t c)
{
	return {
		static_cast<uint8_t>((RGBA_RED_MASK   & c) >> RGBA_RED_BITSHIFT),
		static_cast<uint8_t>((RGBA_GREEN_MASK & c) >> RGBA_GREEN_BITSHIFT),
		static_cast<uint8_t>((RGBA_BLUE_MASK  & c) >> RGBA_BLUE_BITSHIFT),
		static_cast<uint8_t>((RGBA_ALPHA_MASK & c) >> RGBA_ALPHA_BITSHIFT)
	};
}

Color Color::from_argb_bytes(uint32_t c)
{
	return {
		static_cast<uint8_t>((SDL_RED_MASK   & c) >> SDL_RED_BITSHIFT),
		static_cast<uint8_t>((SDL_GREEN_MASK & c) >> SDL_GREEN_BITSHIFT),
		static_cast<uint8_t>((SDL_BLUE_MASK  & c) >> SDL_BLUE_BITSHIFT),
		static_cast<uint8_t>((SDL_ALPHA_MASK & c) >> SDL_ALPHA_BITSHIFT)
	};
}

std::string Color::to_hex_string() const
{
	std::ostringstream os;

	os << "#"
		<< std::hex << std::setfill('0')
		<< std::setw(2) << static_cast<int>(r)
		<< std::setw(2) << static_cast<int>(g)
		<< std::setw(2) << static_cast<int>(b);

	return os.str();
}

std::string Color::to_rgba_string() const
{
	std::ostringstream os;
	
	os << static_cast<int>(r) << ','
		<< static_cast<int>(g) << ','
		<< static_cast<int>(b) << ','
		<< static_cast<int>(a);

	return os.str();
}

std::string Color::to_rgb_string() const
{
	std::ostringstream os;

	os << static_cast<int>(r) << ','
		<< static_cast<int>(g) << ','
		<< static_cast<int>(b);

	return os.str();
}
