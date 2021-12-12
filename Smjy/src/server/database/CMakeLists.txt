CollectSourceFiles(
  ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE_SOURCES
  # Exclude
  ${CMAKE_CURRENT_SOURCE_DIR}/PrecompiledHeaders)

if(USE_COREPCH)
  set(PRIVATE_PCH_HEADER PrecompiledHeaders/databasePCH.h)
endif()

GroupSources(${CMAKE_CURRENT_SOURCE_DIR})

add_library(database
  ${PRIVATE_SOURCES}
)

# Do NOT add any extra include directory unless it does not create unneeded extra dependencies,
# and specially, not add any dependency to neither of these: shared, game, scripts
# This way we ensure that if either a PR does that without modifying this file,
# a compile error will be generated, either this file will be modified so it
# is detected more easily.
# While it is OK to include files from other libs as long as they don't require
# linkage (enums, defines...) it is discouraged to do so unless necessary, as it will pullute
# include_directories leading to further unnoticed dependency aditions
# Linker Depencency requirements: common不要添加任何额外的包含目录，除非它不会创建不需要的额外依赖项，特别是，不要向以下任何一个添加任何依赖项：shared、game、scripts 这样我们可以确保如果一个 PR 在不修改此文件的情况下执行此操作， 将产生编译错误，要么修改此文件，以便更容易地检测到它。 虽然可以包含来自其他库的文件，只要它们不需要链接（枚举、定义...），除非必要，否则不鼓励这样做，因为它会拉取 include_directories 导致进一步不被注意的依赖项添加链接器依赖要求 ： 常见的 
CollectIncludeDirectories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC_INCLUDES
  # Exclude
  ${CMAKE_CURRENT_SOURCE_DIR}/PrecompiledHeaders)

target_include_directories(database
  PUBLIC
    ${PUBLIC_INCLUDES}
  PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR})

add_definitions(-DTRINITY_API_EXPORT_DATABASE)

target_link_libraries(database
  PRIVATE
    trinity-core-interface
    mysql
  PUBLIC
    common)

set_target_properties(database
    PROPERTIES
      FOLDER
        "server")

if(BUILD_SHARED_LIBS)
  if(UNIX)
    install(TARGETS database
      LIBRARY
        DESTINATION lib)
  elseif(WIN32)
    install(TARGETS database
      RUNTIME
        DESTINATION "${CMAKE_INSTALL_PREFIX}")
  endif()
endif()

# Generate precompiled header
if(USE_COREPCH)
  add_cxx_pch(database ${PRIVATE_PCH_HEADER})
endif()
