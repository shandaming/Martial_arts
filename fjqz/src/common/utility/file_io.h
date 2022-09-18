//
// Created by mylw on 22-9-18.
//

#ifndef FJQZ_FILE_IO_H
#define FJQZ_FILE_IO_H

#include <string_view>
#include <vector>

extern std::string current_path;

std::vector<char> load_file_data(std::string_view file);

std::string get_path_key(std::string_view file);
std::string get_filename(std::string_view file);

std::vector<std::string> get_directory_files(std::string_view path);
std::vector<int> get_mmap_file_index(const std::vector<std::string>& files);

#endif //FJQZ_FILE_IO_H
