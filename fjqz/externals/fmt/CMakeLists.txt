
include(CheckSymbolExists)

set(strtod_l_headers stdlib.h)

if(WIN32)
  check_symbol_exists(_strtod_l "${strtod_l_headers}" HAVE_STRTOD_L)
else()
  check_symbol_exists(strtod_l "${strtod_l_headers}" HAVE_STRTOD_L)
endif()

set(FMT_HEADERS
  include/fmt/chrono.h
  include/fmt/color.h
  include/fmt/compile.h
  include/fmt/core.h
  include/fmt/format.h
  include/fmt/format-inl.h
  include/fmt/locale.h
  include/fmt/os.h
  include/fmt/ostream.h
  include/fmt/posix.h
  include/fmt/printf.h
  include/fmt/ranges.h)

set(FMT_SOURCES
  src/format.cc
  src/os.cc)

add_library(fmt STATIC ${FMT_SOURCES} ${FMT_HEADERS})

if (HAVE_STRTOD_L)
  target_compile_definitions(fmt 
    PUBLIC 
      FMT_LOCALE)
endif()

target_include_directories(fmt
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(fmt
  PRIVATE
    fjqz-externals-interface)

set_target_properties(fmt
  PROPERTIES
    FOLDER
      "dep")
