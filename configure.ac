#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([FULL-PACKAGE-NAME], [VERSION], [BUG-REPORT-ADDRESS])
AM_INIT_AUTOMAKE
#AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_SRCDIR([src])
AC_CONFIG_HEADERS([config.h])


# Checks for programs.
AC_PROG_CXX

AC_ARG_ENABLE(debug,
	AS_HELP_STRING([--enable-debug],[enable DEBUG mode(default=no)]),
	[],
	[enable_debug=no])
AS_IF([test "$enable_debug" = "yes"],[CXXFLAGS="-g -O0"],
	[test "$enable_debug" = "no"],[CXXFLAGS="-O2"],
	[])

AC_PROG_CC

# Checks for libraries.

# Chekcs for SDL2
# AC_SEARCH_LIBS(SDL_Init, SDL2, , AC_MSG_ERROR([Unable to find SDL2 library.])
AC_CHECK_LIB(SDL2, SDL_Init,, AC_MSG_ERROR([Unable to find SDL2 library]))
AC_CHECK_LIB(SDL2_image, IMG_Init,, AC_MSG_ERROR([Unable to find SDL2_image library.]))
AC_CHECK_LIB(SDL2_mixer, Mix_Init,, AC_MSG_ERROR([Unable to find SDL2_mixer library.]))
AC_CHECK_LIB(SDL2_ttf, TTF_Init,, AC_MSG_ERROR([Unable to find SDL2_ttf library.]))

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h strings.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([atexit floor memset pow strcasecmp])

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
