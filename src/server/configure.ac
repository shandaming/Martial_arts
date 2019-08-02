#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([FULL-PACKAGE-NAME], [VERSION], [BUG-REPORT-ADDRESS])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

AC_ARG_ENABLE(debug,
	AS_HELP_STRING([--enable-debug],[enable DEBUG mode(default=no)]),
	[],
	[enable_debug=no])
AS_IF([test "$enable_debug" = "yes"],[CXXFLAGS="-g -O0"],
	[test "$enable_debug" = "no"],[CXXFLAGS="-O3"],
	[])

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h netdb.h netinet/in.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([memset socket])
AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
