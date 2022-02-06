find_package(GTest REQUIRED)

if (NOT GTEST_FOUND)
  message(FATAL_ERROR "GTest wasn't found on your system but it's required to build the unit test!")
endif()

if(GTEST_FOUND)
  message(STATUS "Found GTest library: ${GTEST_LIBRARY}")
  message(STATUS "Found GTest headers: ${GTEST_INCLUDE_DIR}")
endif()

add_library(gtest INTERFACE)

target_link_libraries(gtest
  INTERFACE
    ${GTEST_LIBRARY})

target_include_directories(gtest
  INTERFACE
    ${GTEST_INCLUDE_DIR})
