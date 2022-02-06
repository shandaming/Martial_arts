find_package(SDL2_ttf REQUIRED)

if(NOT SDL2_TTF_FOUND)
    message(FATAL_ERROR "SDL2_ttf library not found.")
endif()

if(SDL2_TTF_FOUND)
  message(STATUS "Found SDL2_ttf library: ${SDL2_TTF_LIBRARY}")
  message(STATUS "Found SDL2_ttf headers: ${SDL2_TTF_INCLUDE_DIR}")
endif()

add_library(sdl2_ttf INTERFACE)

target_link_libraries(sdl2_ttf
  INTERFACE
    ${SDL2_TTF_LIBRARY})

target_include_directories(sdl2_ttf
  INTERFACE
    ${SDL2_TTF_INCLUDE_DIR})
