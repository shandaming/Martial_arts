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
  PRIVATE_SOURCES)

list(APPEND PRIVATE_SOURCES ${sources_windows})

GroupSources(${CMAKE_CURRENT_SOURCE_DIR})

# 制作可执行文件
add_executable(fjqz
  ${PRIVATE_SOURCES}
)

if(NOT WIN32)
  set_target_properties(fjqz PROPERTIES
    COMPILE_DEFINITIONS _TRINITY_CORE_CONFIG="${CONF_DIR}/worldserver.conf"
  )
endif()

if(UNIX AND NOT NOJEM AND NOT APPLE)
  set(fjqz_LINK_FLAGS "-pthread ${fjqz_LINK_FLAGS}")
endif()

# set_target_properties 可以设置编译器标志、其他变量等目标属性
set_target_properties(fjqz PROPERTIES LINK_FLAGS "${fjqz_LINK_FLAGS}")

target_link_libraries(fjqz
  PRIVATE
    fjqz-core-interface
  PUBLIC
    fmt
    json)

CollectIncludeDirectories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC_INCLUDES)

target_include_directories(fjqz
  PUBLIC
    ${PUBLIC_INCLUDES}
  PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR})

set_target_properties(fjqz
    PROPERTIES
      FOLDER
        "client")


if(UNIX)
  install(TARGETS fjqz DESTINATION bin)
  if(COPY_CONF)
    install(FILES fjqz.conf.dist DESTINATION ${CONF_DIR})
  endif()
endif()

# Generate precompiled header
if(USE_COREPCH)
  add_cxx_pch(fjqz ${PRIVATE_PCH_HEADER})
endif()
