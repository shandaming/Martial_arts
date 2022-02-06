set(SDL2_MIXER_FOUND 0)

find_path(SDL2_MIXER_INCLUDE_DIR
  NAMES
    SDL_mixer.h
  PATHS
    /usr/include/SDL2
  DOC
    "Specify the directory containing SDL_mixer.h."
)

find_library(SDL2_MIXER_LIBRARY
  NAMES
    SDL2_mixer
  PATHS
    /usr/lib
    /usr/local
	/usr/lib/x86_64-linux-gnu
  DOC
    "Specify the location of the SDL2_mixer library here."
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SDL2_mixer
  REQUIRED_VARS
    SDL2_MIXER_INCLUDE_DIR
    SDL2_MIXER_LIBRARY
  )

mark_as_advanced(SDL2_MIXER_FOUND SDL2_MIXER_INCLUDE_DIR SDL2_MIXER_LIBRARY)
