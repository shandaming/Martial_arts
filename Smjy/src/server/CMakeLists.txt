# Enforce compileparameters for corebuilds under GCC
# This to stop a few silly crashes that could have been avoided IF people
# weren't doing some -O3 psychooptimizations etc.在 GCC 下为 corebuilds 强制执行编译参数这可以阻止一些愚蠢的崩溃，如果人们没有做一些 -O3 心理优化等，这些崩溃是可以避免的。 

if(CMAKE_COMPILER_IS_GNUCXX AND NOT MINGW)
  add_definitions(-fno-delete-null-pointer-checks)
endif()

if(WIN32)
  list(APPEND sources_windows
    ${CMAKE_SOURCE_DIR}/src/common/Platform/ServiceWin32.cpp
    ${CMAKE_SOURCE_DIR}/src/common/Platform/ServiceWin32.h
  )
endif(WIN32)

add_subdirectory(database)
add_subdirectory(shared)
add_subdirectory(game)
add_subdirectory(authserver)
add_subdirectory(scripts)
add_subdirectory(worldserver)
