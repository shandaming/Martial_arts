set(GTEST_FOUND 0)

find_path(GTEST_INCLUDE_DIR
  NAMES
    gtest.h
  PATHS
    /usr/include/gtest
  DOC
    "Specify the directory containing gtest.h."
)

LOG_DEBUG("FindGTest Found GTest path = ${GTEST_INCLUDE_DIR}")

find_library(GTEST_LIBRARY
  NAMES
    libgtest.a
    libgtest.so
  PATHS
    /usr/lib
    /usr/local
  DOC
    "Specify the location of the gtest library here."
)

LOG_DEBUG("FindGTest Found GTest library: ${GTEST_LIBRARY}")


include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GTest
  REQUIRED_VARS
    GTEST_INCLUDE_DIR
    GTEST_LIBRARY
  )

mark_as_advanced(GTEST_FOUND GTEST_INCLUDE_DIR GTEST_LIBRARY)

LOG_DEBUG("FindGTest ${GTEST_FOUND}")

if(GTEST_FOUND)
  message(STATUS "Found GTest library: ${GTEST_LIBRARY}")
  message(STATUS "Found GTest headers: ${GTEST_INCLUDE_DIR}")
endif()
