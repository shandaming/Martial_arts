//
// Created by mylw on 22-9-18.
//

#include <filesystem>
#include <SDL2/SDL_image.h>
#include "sdl_utils.h"
#include "log.h"
#include "video.h"
#include "errors.h"

SDL_Texture* load_texture(const std::string& filename)
{
	namespace fs = std::filesystem;

	auto path = fs::absolute(filename);
	if(!fs::exists(path))
	{
		LOG_ERROR("%s not exist.", filename.data());
		return nullptr;
	}

	LOG_TRACE("Loading %s ...", filename.data());

	SDL_Texture* texture = IMG_LoadTexture(VIDEO->get_renderer(), path.c_str());
	FATAL(texture, "Failed to load texture '%s'", filename.data());

	return texture;
}