AUTOMAKE_OPTIONS=foreign subdir-objects

bin_PROGRAMS=Martial_arts_new_legend
Martial_arts_new_legend_SOURCES=main.cpp 		        \
				log.cpp                         \
                                events.cpp                      \
                                game_config.cpp                 \
                                version.cpp                     \
                                filesystem.cpp                  \
                                commandline_options.cpp         \
                                cmdln.cpp                       \
                                game_launcher.cpp               \
                                config.cpp                      \
                                video.cpp                       \
                                color.h                         \
                                tooltips.cpp                    \
                                                                \
                                serialization/string_utils.cpp  \
                                serialization/unicode.cpp       \
                                                                \
                                font/font_config.cpp            \
                                font/marked-up_text.cpp         \
                                font/sdl_ttf.cpp                \
                                font/text.cpp                   \
                                font/text_cache.cpp             \
                                                                \
                                sdl/surface.cpp                 \
                                sdl/exception.cpp               \
                                sdl/rect.cpp                    \
                                sdl/texture.cpp                 \
                                sdl/utils.cpp                   \
                                sdl/window.cpp                  \
                                sdl/point.cpp

Martial_arts_new_legend_LDADD = -lstdc++fs 

AM_CXXFLAGS=-std=c++17
