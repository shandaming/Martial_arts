//
// Created by mylw on 22-9-10.
//

#ifndef FJQZ_MAIN_SCENE_H
#define FJQZ_MAIN_SCENE_H

#include <string_view>

class texture;

enum object_type
{
	NONE,
	WATER,
	WOOD,
	STONE,
};

struct grid
{
	grid() : can_walk(false), texture(nullptr), type(NONE)
	{}

	bool can_walk;
	texture* texture;
	object_type type;
};

// texture_mgr

struct texture_group
{
	texture_group(); x(0), y(0) {}

	texture_group(const texture_group&& l) :
		textures(std::move(l.textures)),
		position(std::move(l.position))
	{}

	texture_group& operator=(const texture_group&& l)
	{
		if (&l != this)
		{
			textures = std::move(l.textures);
			position = std::move(l.position);
		}
		return *this;
	}

	std::vector<texture> textures;
	point position;
};

#include <map>
class texture_mgr
{
public:
	static texture_mgr* instance();

	void init(std::string_view file);

	texture* get_texture(std::string_view file, int pic);
private:
	texture_mgr();
	~texture_mgr();
	texture_mgr& texture_mgr(const texture_mgr&) = delete;
	texture_mgr& operator=(const texture_mgr&) = delete;

	std::map<std::string, std::vector<texture_group>> texture_group_;
};

#define TEXTURE_MGR texture_mgr::instance()

#endif //FJQZ_MAIN_SCENE_H
