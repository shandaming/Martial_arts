//
// Created by mylw on 2022/5/1.
//

#ifndef FJQZ_UITL_H
#define FJQZ_UITL_H

#include <string_view>

bool string_equal(std::string_view a, std::string_view b);
bool string_to_bool(std::string_view str);

auto load_file_data(std::string_view file);

#endif //FJQZ_UITL_H
