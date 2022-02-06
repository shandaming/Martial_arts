find_package(GMock REQUIRED)

if (NOT GMOCK_FOUND)
  message(FATAL_ERROR "GMock wasn't found on your system but it's required to build the unit test!")
endif()

if(GMOCK_FOUND)
  message(STATUS "Found GMock library: ${GMOCK_LIBRARY}")
  message(STATUS "Found GMock headers: ${GMOCK_INCLUDE_DIR}")
endif()

add_library(gmock INTERFACE)

target_link_libraries(gmock
  INTERFACE
    ${GMOCK_LIBRARY})

target_include_directories(gmock
  INTERFACE
    ${GMOCK_INCLUDE_DIR})
