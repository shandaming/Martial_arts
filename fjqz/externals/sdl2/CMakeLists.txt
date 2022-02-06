find_package(SDL2 REQUIRED)

if(NOT SDL2_FOUND)
    message(FATAL_ERROR "SDL2 library not found.")
endif()

if(SDL2_FOUND)
  message(STATUS "Found SDL2 library: ${SDL2_LIBRARIES}")
  message(STATUS "Found SDL2 headers: ${SDL2_INCLUDE_DIRS}")
endif()

add_library(sdl2 INTERFACE)

target_link_libraries(sdl2
  INTERFACE
    ${SDL2_LIBRARIES})

target_include_directories(sdl2
  INTERFACE
    ${SDL2_INCLUDE_DIRS})
