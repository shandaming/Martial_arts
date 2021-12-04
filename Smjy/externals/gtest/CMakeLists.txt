if (NOT GTest_FOUND)
  message(FATAL_ERROR "GTest wasn't found on your system but it's required to build the unit test!")
endif()

add_library(gtest STATIC IMPORTED GLOBAL)

set_target_properties(gtest
  PROPERTIES
    IMPORTED_LOCATION
      "${GTEST_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES
      "${GTEST_INCLUDE_DIR}")
