#ifndef DEB_DEBUGGING_ERRORS_H
#define DEB_DEBUGGING_ERRORS_H

#include "define.h"

void assert(const char* file, int line, const char* function, const char* message) ATTR_NORETURN;

void assert(const char* file, int line, const char* function, const char* message, const char* format, ...) ATTR_NORETURN ATTR_PRINTF(5, 6);

void fatal(const char* file, int line, const char* function, const char* message, ...) ATTR_NORETURN ATTR_PRINTF(4, 5);

void error(const char* file, int line, const char* function, const char* message) ATTR_NORETURN;

void abort(const char* file, int line, const char* function) ATTR_NORETURN;

void warning(const char* file, int line, const char* function, const char* message);

void abort_handler(int sigval) ATTR_NORETURN;

#define ASSERT(cond, ...) do { if(!(cond))	assert(__FILE__, __LINE__, __FUNCTION__, #cond, ##__VA_ARGS__);	}while(0)

#define FATAL(cond, ...) do { if(!(cond)) fatal(__FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); } while(0)

#define ERROR(cond, msg) do { if(!(cond)) error(__FILE__, __LINE__, __FUNCTION__, (msg)); } while(0)

#define WARNING(cond, msg) do { if(!(cond)) warning(__FILE__, __LINE__, __FUNCTION__, (msg)); } while(0)

#define ABORT() do { abort(__FILE__, __LINE__, __FUNCTION__); } while(0)

template<typename T>
inline T* assert_notnull_impl(T* pointer, const char* expr)
{
	ASSERT(pointer, "%s", expr);
	return pointer;
}

#define ASSERT_NOTNULL(pointer) assert_notnull_impl(pointer, #pointer)

#endif
