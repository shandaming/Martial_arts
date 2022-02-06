set(GMOCK_FOUND 0)

find_path(GMOCK_INCLUDE_DIR
  NAMES
    gmock.h
  PATHS
    /usr/include/gmock
  DOC
    "Specify the directory containing gmock.h."
)

find_library(GMOCK_LIBRARY
  NAMES
    gmock
  PATHS
    /usr/lib
    /usr/local
  DOC
    "Specify the location of the gmock library here."
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GMock
  REQUIRED_VARS
    GMOCK_INCLUDE_DIR
    GMOCK_LIBRARY
  )

mark_as_advanced(GMOCK_FOUND GMOCK_INCLUDE_DIR GMOCK_LIBRARY)
