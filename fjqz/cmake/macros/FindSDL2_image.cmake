set(SDL2_IMAGE_FOUND 0)

find_path(SDL2_IMAGE_INCLUDE_DIR
  NAMES
    SDL_image.h
  PATHS
    /usr/include/SDL2
  DOC
    "Specify the directory containing SDL_image.h."
)

find_library(SDL2_IMAGE_LIBRARY
  NAMES
    SDL2_image
  PATHS
    /usr/lib
    /usr/local
	/usr/lib/x86_64-linux-gnu
  DOC
    "Specify the location of the SDL2_image library here."
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SDL2_image
  REQUIRED_VARS
    SDL2_IMAGE_INCLUDE_DIR
    SDL2_IMAGE_LIBRARY
  )

mark_as_advanced(SDL2_IMAGE_FOUND SDL2_IMAGE_INCLUDE_DIR SDL2_IMAGE_LIBRARY)
