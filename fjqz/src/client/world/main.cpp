/*
 * Copyright (C) 2019
 */

#include <filesystem>

namespace fs = std::filesystem;

#define CONFIG_FILE "jy.cfg"


int main(int argc, char* argv[])
{
    auto config_file = fs::absolute(CONFIG_FILE);


    return 0;
}
