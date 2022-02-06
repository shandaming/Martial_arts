CollectSourceFiles(
  ${CMAKE_CURRENT_SOURCE_DIR}
  TEST_SOURCES
)

GroupSources(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(tests ${TEST_SOURCES})

target_link_libraries(tests
  PRIVATE
    fjqz-core-interface
  PUBLIC
    gtest
    gmock)

set(tests_LINK_FLAGS "-pthread ${tests_LINK_FLAGS}")

set_target_properties(tests PROPERTIES LINK_FLAGS "${tests_LINK_FLAGS}")


CollectIncludeDirectories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  TEST_INCLUDES)

target_include_directories(tests
  PUBLIC
    ${TEST_INCLUDES}
  PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR})

set_target_properties(tests
    PROPERTIES
      FOLDER
        "tests")
