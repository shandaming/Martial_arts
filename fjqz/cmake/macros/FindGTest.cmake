set(GTEST_FOUND 0)

find_path(GTEST_INCLUDE_DIR
  NAMES
    gtest.h
  PATHS
    /usr/include/gtest
  DOC
    "Specify the directory containing gtest.h."
)

find_library(GTEST_LIBRARY
  NAMES
    gtest
  PATHS
    /usr/lib
    /usr/local
  DOC
    "Specify the location of the gtest library here."
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GTest
  REQUIRED_VARS
    GTEST_INCLUDE_DIR
    GTEST_LIBRARY
  )

mark_as_advanced(GTEST_FOUND GTEST_INCLUDE_DIR GTEST_LIBRARY)
