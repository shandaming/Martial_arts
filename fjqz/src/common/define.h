#ifndef _DEFINE_H
#define _DEFINE_H

#include "compiler_defs.h"

#if COMPILER == COMPILER_GNU
	#if !defined(__STDC_FORMAT_MACROS)
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

	#define DECLSPEC_NORETURN
	#define DECLSPEC_DEPRECATED


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

#endif
