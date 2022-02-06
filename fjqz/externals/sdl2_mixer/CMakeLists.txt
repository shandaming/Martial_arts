find_package(SDL2_mixer REQUIRED)

if(NOT SDL2_MIXER_FOUND)
    message(FATAL_ERROR "SDL2_mixer library not found.")
endif()

if(SDL2_MIXER_FOUND)
  message(STATUS "Found SDL2_mixer library: ${SDL2_MIXER_LIBRARY}")
  message(STATUS "Found SDL2_mixer headers: ${SDL2_MIXER_INCLUDE_DIR}")
endif()

add_library(sdl2_mixer INTERFACE)

target_link_libraries(sdl2_mixer
  INTERFACE
    ${SDL2_MIXER_LIBRARY})

target_include_directories(sdl2_mixer
  INTERFACE
    ${SDL2_MIXER_INCLUDE_DIR})
