# This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

CollectSourceFiles(
  ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE_SOURCES
  # Exclude
  ${CMAKE_CURRENT_SOURCE_DIR}/Debugging
  ${CMAKE_CURRENT_SOURCE_DIR}/Platform
  ${CMAKE_CURRENT_SOURCE_DIR}/PrecompiledHeaders)

LOG_DEBUG("common CMAKE_CURRENT_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR}; PRIVATE_SOURCES = ${PRIVATE_SOURCES}")

# Manually set sources for Debugging directory as we don't want to include WheatyExceptionReport in common project
# It needs to be included both in authserver and worldserver for the static global variable to be properly initialized
# and to handle crash logs on windows手动设置调试目录的源，因为我们不想在公共项目中包含 WheatyExceptionReport 它需要包含在 authserver 和 worldserver 中才能正确初始化静态全局变量并处理 Windows 上的崩溃日志
list(APPEND PRIVATE_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/Debugging/Errors.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Debugging/Errors.h)

LOG_DEBUG("common PRIVATE_SOURCES = ${PRIVATE_SOURCES}")

if(USE_COREPCH)
  set(PRIVATE_PCH_HEADER PrecompiledHeaders/commonPCH.h)
endif(USE_COREPCH)

GroupSources(${CMAKE_CURRENT_SOURCE_DIR})

add_definitions(-DTRINITY_API_EXPORT_COMMON)

LOG_DEBUG("common DTRINITY_API_EXPORT_COMMON = ${DTRINITY_API_EXPORT_COMMON}")

add_library(common
  ${PRIVATE_SOURCES}
)

LOG_DEBUG("common common = ${common}")

# Do NOT add any extra include directory here, as we don't want the common
# library to depend on anything else than TC deps, and itself.
# This way we ensure that if either a PR does that without modifying this file,
# a compile error will be generated, either this file will be modified so it
# is detected more easily.
# While it is OK to include files from other libs as long as they don't require
# linkage (enums, defines...) it is discouraged to do so unless necessary, as it will pullute
# include_directories leading to further unnoticed dependency aditions
# Linker Depencency requirements: none不要在此处添加任何额外的包含目录，因为我们不希望公共库依赖于 TC deps 及其本身以外的任何其他内容。 这样我们可以确保如果 PR 在不修改此文件的情况下执行此操作，则会生成编译错误，或者将修改此文件以便更容易地检测到它。 虽然可以包含来自其他库的文件，只要它们不需要链接（枚举、定义...），除非必要，否则不鼓励这样做，因为它会拉取 include_directories 导致进一步不被注意的依赖项添加链接器依赖要求 ： 没有任何
CollectIncludeDirectories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC_INCLUDES
  # Exclude
  ${CMAKE_CURRENT_SOURCE_DIR}/PrecompiledHeaders)

LOG_DEBUG("common PUBLIC_INCLUDES = ${PUBLIC_INCLUDES}")

target_include_directories(common
  PUBLIC
    # Provide the binary dir for all child targets为所有子目标提供二进制目录 
    ${CMAKE_BINARY_DIR}
    ${PUBLIC_INCLUDES}
  PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR})

LOG_DEBUG("common CMAKE_BINARY_DIR = ${CMAKE_BINARY_DIR}; CMAKE_CURRENT_BINARY_DIR = ${CMAKE_CURRENT_BINARY_DIR}; common = ${common}")

target_link_libraries(common
  PRIVATE
    trinity-core-interface
  PUBLIC
    argon2
    boost
    fmt
    # g3dlib
    # Detour
    sfmt
    utf8cpp
    openssl
    valgrind
    threads
    jemalloc
    gtest
    gmock)

LOG_DEBUG("common trinity-core-interface = ${trinity-core-interface}")

# 添加依赖项 
add_dependencies(common revision_data.h)

set_target_properties(common
    PROPERTIES
      FOLDER
        "server")
        
LOG_DEBUG("common FOLDER = ${FOLDER}")

if(BUILD_SHARED_LIBS)
  if(UNIX)
    install(TARGETS common
      LIBRARY
        DESTINATION lib)
  elseif(WIN32)
    install(TARGETS common
      RUNTIME
        DESTINATION "${CMAKE_INSTALL_PREFIX}")
  endif()
endif()

# Generate precompiled header
if(USE_COREPCH)
  add_cxx_pch(common ${PRIVATE_PCH_HEADER})
endif()
