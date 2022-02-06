find_package(SDL2_image REQUIRED)

if(NOT SDL2_IMAGE_FOUND)
    message(FATAL_ERROR "SDL2_image library not found.")
endif()

if(SDL2_IMAGE_FOUND)
  message(STATUS "Found SDL2_image library: ${SDL2_IMAGE_LIBRARY}")
  message(STATUS "Found SDL2_image headers: ${SDL2_IMAGE_INCLUDE_DIR}")
endif()

add_library(sdl2_image INTERFACE)

target_link_libraries(sdl2_image
  INTERFACE
    ${SDL2_IMAGE_LIBRARY})

target_include_directories(sdl2_image
  INTERFACE
    ${SDL2_IMAGE_INCLUDE_DIR})
