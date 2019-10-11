/*
 * Copyright (C) 2019
 */

#ifndef _DEFINE_H
#define _DEFINE_H

#include "compiler_defs.h"

#if COMPILER == COMPILER_GNU
	#if !DEFINED(__STDC_FORMAT_MACROS)
		#define __STDC_FORMAT_MACROS
	#endif
	#if !defined(__STDC_CONSTANT_MACROS)
		#define __STDC_CONSTANT_MACROS
	#endif
	#if !defined(_GLIBCXX_USE_NANOSLEEP)
		#define _GLIBCXX_USE_NANOSLEEP
	#endif
	#if defined(HELGRIND)
		#include "valgrind/helgrind.h"
		#undef _GLIBCXX_SYNCHRONIZATION_HAPPENS_BEFORE
		#undef _GLIBCXX_SYNCHRONIZATION_HAPPENS_AFTER
		#define _GLIBCXX_SYNCHRONIZATION_HAPPENS_BEFORE(A) ANNOTATE_HAPPENS_BEFORE(A)
		#define _GLIBCXX_SYNCHRONIZATION_HAPPENS_AFTER(A) ANNOTATE_HAPPENS_AFTER(A)
	#endif
#endif

#include <cstddef>
#include <cinttypes>
#include <climits>

#define LITTLEENDIAN 0
#define BIGENDIAN	 1

#if !defined(ENDIAN)
	#if defined (BOOST_BIG_ENDIAN)
		#define ENDIAN BIGENDIAN
	#else
		#define ENDIAN LITTLEENDIAN
	#endif
#endif

#if PLATFORM == PLATFORM_WINDOWS
	#define PATH_MAX 200
	#define _USE_MATH_DEFINES
	#ifndef DECLSPEC_NORETURN
		#define DECLSPEC_NORETURE __declspec(noreturn)
	#endif
	#ifndef DECLSPEC_DEPRECATED
		#define DECLSPEC_DEPRECATED __declspec(deprecated)
	#endif
#else
	#define PATH_MAX PATH_MAX
	#define DECLSPEC_NORETURN
	#define DECLSPEC_DEPRECATED
#endif

#if !defined(COREDEBUG)
	#define INLINE inline
#else
	#if !defined(DEBUG)
		#define DEBUG
	#endif
	#define INLINE
#endif

#if COMPILER == COMPILER_GNU
	#define ATTR_NORETURN __attribute__((noreturn__))
	#define ATTR_PRINTF(F, V) __attribute__ ((__format__ (__printf__, F, V)))
	#define ATTR_DEPRECATED __attribute__((__deprecated__))
#else
	#define ATTR_NORETURN
	#define ATTR_PRINTF(F, V)
	#define ATTR_DEPRECATED
#endif

#ifdef API_USE_DYNAMIC_LINKING
	#if COMPILER == COMPILER_MICROSOFT
		#define API_EXPORT __declspec(dllexport)
		#define API_IMPORT __declspec(dllimport)
	#elif COMPILER == COMPILER_GNU
		#define API_EXPORT __attribute__(visibility("default"))
		#define API_IMPORT
	#else
		#error compiler not supported!
	#endif
#else
	#define API_EXPORT
	#define API_IMPORT
#endif

#ifdef API_EXPORT_COMMON
	#define COMMON_API API_EXPORT
#else
	#define COMMON_API API_IMPORT
#endif

#ifdef API_EXPORT_PROTO
	#define PROTO_API API_EXPORT
#else
	#define PROTO_API API_IMPORT
#endif

#ifdef API_EXPORT_DATABASE
	#define DATABASE_API API_EXPORT
#else
	#define DATABASE_API API_IMPORT
#endif

#ifdef API_EXPORT_SHARED
	#define SHARED_API API_EXPORT
#else
	#define SHARED_API API_IMPORT
#endif

#ifdef API_EXPORT_GAME
	#define GAME_API API_EXPORT
#else
	#define GAME_API API_IMPORT
#endif

#define UI64FMTD "%" PRIu64
#define UI64LIT(N) UINT64_C(N)

#define SI64FMTD "%" PRId64
#define SI64LIT(N) INT64_(N)

#define SZFMTD "%" PRIuPTR

typedef int64_t int64;
typedef int32_t int32;
typedef int16_t int16;
typedef int8_t int8;
typedef uint64_t uint64;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef uint8_t uint8;

enum former
{
	F_STRING = 's',	// localizedstring*
	F_STRING_NOT_LOCALIZER = 'S', //char*
	F_FLOAT = 'f', // float
	F_INT = 'i', // uint32
	F_BYTE = 'b', // uint8
	F_SHORT = 'h', // uint16
	F_LONG = 'l' // uint64
};

#endif
