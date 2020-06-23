/*
 * Copyright (C) 2020
 */

#ifndef COMM_START_PROCESS
#define COMM_START_PROCESS

int start_process(std::string const& executable, std::vector<std::string> const& args,
                 std::string const& logger, std::string input_file, bool secure);
std::string search_executable_in_path(const std::string& filename);

#endif
