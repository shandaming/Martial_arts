if (NOT GMOCK_FOUND)
  message(FATAL_ERROR "GTest wasn't found on your system but it's required to build the unit test!")
endif()

add_library(gmock STATIC IMPORTED GLOBAL)

set_target_properties(gmock
  PROPERTIES
    IMPORTED_LOCATION
      "${GMOCK_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES
      "${GMOCK_INCLUDE_DIR}")
