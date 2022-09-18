//
// Created by mylw on 22-9-17.
//

#include "texture_mgr.h"
#include "texture_cache.h"
#include "utility/file_io.h"
#include "config.h"
#include "errors.h"
#include "string_format.h"
#include "sdl_utils.h"
#include "point.h"

namespace
{
std::vector<texture_map> mmap_textures_;

auto load_indexka(std::string_view file)
{
	auto data = load_file_data(file);
	ASSERT(data.size() > 0);

	int length = data.size() / sizeof(int16_t);
	std::vector<int16_t> result(length, 0);
	memmove(result.data(), data.data(), length * sizeof(int16_t));

	return result;
}

texture_cache load_texture_cache(const std::string& file)
{
	texture_cache cache;
	texture tex(load_texture(file));
	if (tex)
	{
		cache.add(get_filename(file), std::move(tex));
	}
	else
	{
		for (int j = 0; j < 10; ++j)
		{
			texture tex(load_texture(file));
			if (tex)
				cache.add(get_filename(file), std::move(tex));
		}
	}
	return cache;
}
}

void texture_mgr::load_mmap_textures()
{
	std::string file = CONFIG_MGR->get_resource_path("mmap");

	auto files = get_directory_files(file);
	std::vector<int> index = get_mmap_file_index(files);

	auto offset = load_indexka(file);

	mmap_textures_.resize(offset.size() / 2);

	for (int i = 0; i < mmap_textures_.size(); i++)
	{
		auto &iter = mmap_textures_[i];
		iter.pos.x = offset[i * 2];
		iter.pos.y = offset[i * 2 + 1];

		if (std::find(index.begin(), index.end(), i) != index.end())
		{
			texture_map::texture_array array;
			for (uint32_t j = 0; j < array.size(); ++j)
			{
				texture t(load_texture(string_format("%s%s%d_%d.png", current_path.c_str(), file.c_str(), i, j)));
				array[j] = std::move(t);
			}
			iter.img_texture.emplace<1>(std::move(array));
		} else
		{
			texture t(load_texture(string_format("%s%s%d.png", current_path.c_str(), file.c_str(), i)));
			iter.img_texture.emplace<0>(std::move(t));
		}
	}
}

texture_mgr* texture_mgr::instance()
{
	static texture_mgr instance;
	return &instance;
}