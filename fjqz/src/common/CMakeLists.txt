CollectSourceFiles(
  ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE_SOURCES
  )

GroupSources(${CMAKE_CURRENT_SOURCE_DIR})

add_library(common
  ${PRIVATE_SOURCES}
)


CollectIncludeDirectories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC_INCLUDES
  )

target_include_directories(common
  PUBLIC
    # Provide the binary dir for all child targets为所有子目标提供二进制目录 
    ${CMAKE_BINARY_DIR}
    ${PUBLIC_INCLUDES}
  PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(common
  PRIVATE
    fjqz-core-interface
  PUBLIC
    fmt
    utf8cpp
    threads
    jemalloc
    gtest
    gmock)

set_target_properties(common
    PROPERTIES
      FOLDER
        "client"
        )
