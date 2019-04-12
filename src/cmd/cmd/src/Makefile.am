AUTOMAKE_OPTIONS = foreign subdir-objects

bin_PROGRAMS = world_server

world_server_SOURCES = main.cpp \
					   cmdline,coo \
					   options_description.cpp \
					   positional_options.cpp \
					   value_semantic.cpp \
					   variables_map.cpp
					   
world_server_LDADD = -lstdc++fs 
                         
AM_CXXFLAGS = -std=c++17 -pthread -Wall -Wextra -Werror=non-virtual-dtor -Wno-unused-local-typedefs -Wno-maybe-uninitialized -Wold-style-cast -Wtrampolines
