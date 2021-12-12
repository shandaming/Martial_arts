set(GMOCK_FOUND 0)

find_path(GMOCK_INCLUDE_DIR
  NAMES
    gmock.h
  PATHS
    /usr/include/gmock
  DOC
    "Specify the directory containing gmock.h."
)

LOG_DEBUG("FindGMock Found GMock path = ${GMOCK_INCLUDE_DIR}")

find_library(GMOCK_LIBRARY
  NAMES
    libgmock.a
    libgmock.so
  PATHS
    /usr/lib
    /usr/local
  DOC
    "Specify the location of the gmock library here."
)

LOG_DEBUG("FindGMock Found GMock library: ${GMOCK_LIBRARY}")

#if(GMOCK_INCLUDE_DIR AND EXISTS "${GMOCK_INCLUDE_DIR}/gmock.h")
#endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GMock
  REQUIRED_VARS
    GMOCK_INCLUDE_DIR
    GMOCK_LIBRARY
  )

mark_as_advanced(GMOCK_FOUND GMOCK_INCLUDE_DIR GMOCK_LIBRARY)

LOG_DEBUG("FindGMock ${GMOCK_FOUND}")

if(GMOCK_FOUND)
  message(STATUS "Found GMock library: ${GMOCK_LIBRARY}")
  message(STATUS "Found GMock headers: ${GMOCK_INCLUDE_DIR}")
endif()
