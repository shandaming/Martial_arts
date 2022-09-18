//
// Created by mylw on 22-9-10.
//

#include <cstring>
#include <vector>
#include "main_scene.h"
#include "errors.h"
#include "utf8.h"
#include "utility/uitl.h"

#define MAIN_MAP_COORD_COUNT 480
#define MAIN_MAP_COORD_COUNT_SQUARED (MAIN_MAP_COORD_COUNT * MAIN_MAP_COORD_COUNT)
#define MAIN_MAP_DATA_LENGTH (MAIN_MAP_COORD_COUNT * MAIN_MAP_COORD_COUNT * sizeof(int16_t))

#define BATTLE_MAP_COORD_COUNT 64
#define BATTLE_MAP_SAVE_LAYER_COUNT 2		// 数据文件存储地图数据层数
#define BATTLE_ENEMY_COUNT 20
#define TEAMMATE_COUNT 6          //最大队伍人员数

auto load_main_building_coord(std::string_view file)
{
	auto data = load_file_data(file);
	ASSERT(data.size() >= MAIN_MAP_COORD_COUNT_SQUARED, "Failed to load main map data.");

	std::vector<int16_t> result(MAIN_MAP_COORD_COUNT_SQUARED, 0);
	memmove(result.data(), data.data(), MAIN_MAP_DATA_LENGTH);
	return result;
}

auto load_main_map_data(std::string_view file)
{
	auto result = load_main_building_coord(file);

	std::transform(result.begin(), result.end(), result.begin(),
		[](int16_t value) { return (int16_t)(value / 2); });
	return result;
}


auto load_main_map_texture(std::string_view file)
{
	auto square_map_data = load_main_map_data(file);
	std::vector<grid> result(MAIN_MAP_COORD_COUNT_SQUARED, grid());

	for (int i = 0; i < square_map_data.size(); i++)
	{
		auto pic = square_map_data[i];
		if (pic > 0)
		{
			auto& grid = result[i];
			grid.texture = TextureManager::getInstance()->loadTexture("mmap", pic);

			if (pic == 419 || pic >= 306 && pic <= 335)
			{
				grid.type = WATER;
			}
			else if (pic >= 179 && pic <= 181 || pic >= 253 && pic <= 335 || pic >= 508 && pic <= 511)
			{
				grid.type = WATER;
				grid.can_walk = true;
			}
			else if (pic >= 1008 && pic <= 1164 || pic >= 1214 && pic <= 1238)
			{
				grid.type = WOOD;
			}
		}
	}

	return result;
}

// dialogue_text and event
// dialogue_text
auto load_idx_data(std::string_view file)
{
	auto data = load_file_data(file);
	ASSERT(data.size() > 0, "Failed to load talk_idx data.");

	int length = data.size() / sizeof(int);
	std::vector<int> result(length, 0);
	memmove(result.data(), data.data(), length * sizeof(int));

	return result;
}

auto get_idx_offset(const std::vector<int>& talk_idx)
{
	std::vector<int> offset(talk_idx.size() + 1, 0);
	memcpy(offset.data() + 1, talk_idx.data(), talk_idx.size());
	return offset;
}

auto get_grp_file_data(std::string_view file)
{
	auto data = load_file_data(file);
	ASSERT(data.size() > 0, "Failed to load alk_grp data.");

	std::string result(data.size(), 0);
	memmove(result.data(), data.data(), data.size());

	return result;
}

std::string trim(std::string_view s, char ch)
{
	if (s.empty())
		return std::string();

	std::string result(s.data());
	result.erase(std::remove_if(result.begin(), result.end(),
								[ch](char c) { return c == ch; }), result.end());
	return result;
}

auto load_dialogue_text_entry(std::string_view idx_path, std::string_view grp_path)
{
	auto dialogue_text_idx = load_idx_data(idx_path);
	auto dialogue_text = get_grp_file_data(grp_path);
	auto offset = get_idx_offset(dialogue_text_idx);

	for(uint32_t i = 0; i < offset.back(); ++i)
	{
		if (dialogue_text[i])
			dialogue_text[i] = dialogue_text[i] ^ 0xff;
	}

	std::vector<std::string> talk_entry;
	for(uint32_t i = 0; i < dialogue_text_idx.size(); ++i)
	{
		std::string_view str{dialogue_text.data() + offset[i]};
		std::string entry;
		utf8::utf16to8(str.begin(), str.end(), std::back_inserter(entry));

		talk_entry.emplace_back(trim(entry, '*'));
	}

	return talk_entry;
}

auto get_plot_event_offset_distance(const std::vector<int>& talk_offset)
{
	std::vector<int> result(talk_offset.size() - 1, 0);

	for(uint32_t i = 0; i < result.size(); ++i)
	{
		result[i] = talk_offset[i + 1] - talk_offset[i];
	}

	return result;
}
// event
auto load_game_plot_events(std::string_view idx_path, std::string_view grp_path)
{
	auto plot_idx = load_idx_data(idx_path);
	auto plot_events = get_grp_file_data(grp_path);
	auto offset = get_idx_offset(plot_idx);
	auto offset_distance = get_plot_event_offset_distance(offset);

	std::vector<std::vector<int>> result(offset_distance.size());
	for(uint32_t i = 0; i < offset_distance.size(); ++i)
	{
		int length = offset_distance[i] / sizeof(int16_t);
		result[i].resize(length, -1);

		for(uint32_t j = 0; j < length; ++j)
		{
			result[i][j] = *(int16_t*)(plot_events.data() + offset[i] + j * 2);
		}
	}

	return result;
}

// battle
using battle_map_type = int16_t [BATTLE_MAP_SAVE_LAYER_COUNT][BATTLE_MAP_COORD_COUNT * BATTLE_MAP_COORD_COUNT];

struct battle
{
	int16_t id;
	char name[10];
	int16_t battle_field_id, exp, music;
	int16_t Teammate[TEAMMATE_COUNT], auto_teamMate[TEAMMATE_COUNT], teammate_x[TEAMMATE_COUNT], teammate_y[TEAMMATE_COUNT];
	int16_t enemy[BATTLE_ENEMY_COUNT], enemy_x[BATTLE_ENEMY_COUNT], enemy_y[BATTLE_ENEMY_COUNT];
};

auto load_battle_map(std::string_view idx_path, std::string_view grp_path)
{
	auto battle_idx = load_idx_data(idx_path);
	auto battle_map = get_grp_file_data(grp_path);
	auto offset = get_idx_offset(battle_idx);
	//auto offset_distance = get_plot_event_offset_distance(offset);

	std::vector<battle_map_type> result(battle_idx.size());
	for(uint32_t i = 0; i < result.size(); ++i)
	{
		memcpy(result[i], battle_map.data() + offset[i], sizeof(battle_map_type));
	}
	//int16_t data[BATTLEMAP_SAVE_LAYER_COUNT][BATTLE_MAP_COORD_COUNT * BATTLE_MAP_COORD_COUNT];
	return result;
}

auto load_battle_data(std::string_view file)
{
	auto data = load_file_data(file);
	ASSERT(data.size() > 0, "Failed to load battle data.");

	int count = data.size() / sizeof(battle);
	std::vector<battle> result(count);
	memmove(result.data(), data.data(), count * sizeof(battle));

	for(auto& iter : result)
	{
		std::string_view name {iter.name};
		std::string utf8name;

		utf8::utf16to8(name.begin(), name.end(), std::back_inserter(utf8name));
		memmove(iter.name, utf8name.data(), utf8name.size());
	}

	return result;
}




// texture_mgr
// index.ka
auto load_indexka(std::string_view file)
{
	auto data = load_file_data(file);
	ASSERT(data.size() > 0);

	int length = data.size() / sizeof(int16_t);
	std::vector<int16_t> result(length, 0);
	memmove(result.data(), data.data(), length * sizeof(int16_t));

	return result;
}


void init(std::string_view file)
{
	auto offset = load_indexka(file);
	std::vector<texture_group> texture_list(offset.size() / 2, texture_group());

	for (int i = 0; i < textures.size(); i++)
	{
		texture_list[i].position.x = offset[i * 2];
		texture_list[i].position.y = offset[i * 2 + 1];
		auto& iter = texture_list[i];

		texture tex = loadImage(path_ + "/" + std::to_string(i) + ".png");
		if (tex)
		{
			iter.textures.push_back(std::move(tex));
		}
		else
		{
			for (int j = 0; j < 10 ++j)
			{

				texture tex = loadImage(path_ + "/" + std::to_string(num) + "_" + std::to_string(i) + ".png");
				if (tex)
					iter.textures.push_back(std::move(tex));
			}
		}
	}

	texture_group_[file] = std::move(texture_list);
}

texture* texture_mgr::get_texture(std::string_view file, int pic)
{
	auto iter = texture_group_.find(file);
	if (iter != texture_group_.end())
	{
		return nullptr;
	}

	if(iter->second[pic].texture_frame.size() > 0)
	return &(iter->second[pic]);
}

texture_mgr* texture_mgr::instance()
{
	static texture_mgr instance;
	return &instance;
}