set(SDL2_TTF_FOUND 0)

find_path(SDL2_TTF_INCLUDE_DIR
  NAMES
    SDL_ttf.h
  PATHS
    /usr/include/SDL2
  DOC
    "Specify the directory containing SDL_ttf.h."
)

find_library(SDL2_TTF_LIBRARY
  NAMES
    SDL2_ttf
  PATHS
    /usr/lib
    /usr/local
	/usr/lib/x86_64-linux-gnu
  DOC
    "Specify the location of the SDL2_ttf library here."
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SDL2_ttf
  REQUIRED_VARS
    SDL2_TTF_INCLUDE_DIR
    SDL2_TTF_LIBRARY
  )

mark_as_advanced(SDL2_TTF_FOUND SDL2_TTF_INCLUDE_DIR SDL2_TTF_LIBRARY)
