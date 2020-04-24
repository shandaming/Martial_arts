/*
 * Copyright (C) 2020
 */

#include <unistd.h>

#include <filesystem>
#include <vector>
#include <cstdlib>

#include "start_process.h"
#include "util.h"

namespace fs = std::filesystem;

namespace
{
std::vector<fs::path> path()
{
	const std::string path_env = getenv("PATH");
	tokenizer token(path_env, ';');

	std::vector<fs::path> res;

	for(auto& it : token)
	{
		res.push_back(std::move(fs::path(it)));
	}

	return res;
}
}

inline fs::path search_path(const fs::path& filename, const std::vector<fs::path>& path = path())
{
	for(const fs::path& pp : path)
	{
		auto p = pp / filename;
		std::error_code ec;
		bool file = fs::is_regular_file(p, ec);
		if(!ec && file && ::access(p.c_str(), X_OK) == 0)
			return p;
	}
	return "";
}

std::string search_executable_in_path(const std::string& filename)
{
	return search_path(filename);
}
