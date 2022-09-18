//
// Created by mylw on 22-9-18.
//

#ifndef FJQZ_TEXTURE_CACHE_H
#define FJQZ_TEXTURE_CACHE_H

#include <map>
#include <string>
#include <vector>
#include <array>
#include <variant>
#include <type_traits>
#include "texture.h"
#include "point.h"

class texture_cache
{
public:
	texture_cache();
	texture_cache(texture_cache&& l);
	texture_cache& operator=(texture_cache&& l);

	void add(const std::string& key, texture&& t);
	void remove(const std::string& key);

	texture* get_texture(auto& key) const;
	std::vector<texture*> get_all_textures();
private:
	texture_cache(const texture_cache&) = delete;
	texture_cache& operator=(const texture_cache&) = delete;

	std::map<std::string, texture> cache_;
};

struct texture_map
{
	using texture_array = std::array<texture, 7>;

	template<typename T>
	T* get()
	{
		auto val = std::get_if<texture>(&img_texture);
		if(val)
			return val;
		else
			return std::get_if<texture_array>(&img_texture);
	}

	std::variant<texture, std::array<texture, 7>> img_texture;
	point pos;
};

#endif //FJQZ_TEXTURE_CACHE_H
