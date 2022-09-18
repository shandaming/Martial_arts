//
// Created by mylw on 22-9-18.
//

#include <fstream>
#include <filesystem>

#include "file_io.h"
#include "errors.h"

namespace fs = std::filesystem;

std::string current_path;

std::vector<char> load_file_data(std::string_view file)
{
	const std::string pathfile = current_path + file.data();
	FATAL(fs::exists(pathfile), "%s not exist.", pathfile.c_str());

	std::vector<char> result;

	std::fstream in(pathfile, std::ios::in | std::ios::binary | std::ios::ate);
	if(!in.is_open())
		return result;

	result.resize(in.tellg());
	in.seekg(0, std::ios::beg);

	in.read(result.data(), result.size());

	return result;
}

std::string get_path_key(std::string_view file)
{
	return fs::path(file).parent_path();
}

std::string get_filename(std::string_view file)
{
	return fs::path(file).filename();
}

std::vector<std::string> get_directory_files(std::string_view path)
{
	std::vector<std::string> result;
	for(auto& file : fs::directory_iterator(current_path + path.data()))
	{
		std::string_view filename = file.path().string();
		std::string_view::size_type pos = filename.find(".png");
		if(pos != std::string_view::npos)
		{
			result.push_back(filename.data());
		}
	}
	return result;
}

std::vector<int> get_mmap_file_index(const std::vector<std::string>& files)
{
	std::vector<int> result;
	for(auto& file : files)
	{
		std::string filename = fs::path(file).filename();
		std::string::size_type pos = filename.find("_");
		if (pos != std::string::npos)
			result.push_back(std::stoi(filename.substr(0, pos).data()));
	}
	return result;
}
