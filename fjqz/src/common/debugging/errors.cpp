#include <thread>
#include <cstdarg>
#include <cstdio>

#include "error.h"

void assert(const char* file, int line, const char* function, const char* message)
{
	fprintf(stderr, "\n%s:%d in %s ASSERTION FAILED:\n  %s\n", file, line, function, message);
	*((volatile int*)NULL) = 0;
	exit(1);
}

void assert(const char* file, int line, const char* function, const char* message, const char* format, ...)
{
	va_list args;
	va_start(args, format);

	fprintf(stderr, "\n%s:%d in %s ASSERTION FAILED:\n  %s ", file, line, function, message);
	vfprintf(stderr, format, args);
	fprintf(stderr, "\n");
	fflush(stderr);

	va_end(args);
	*((volatile int*)NULL) = 0;
	exit(1);
}

void fatal(const char* file, int line, const char* function, const char* message, ...)
{
	va_list args;
	va_start(args, message);

	fprintf(stderr, "\n%s:%d in %s ASSERTION ERROR:\n  ", file, line, function);
	vfprintf(stderr, message, args);
	fprintf(stderr, "\n");
	fflush(stderr);

	std::this_thread::sleep_for(std::chrono::seconds(10));
	*((volatile int*)NULL) = 0;
	exit(1);
}

void error(const char* file, int line, const char* function, const char* message)
{
	fprintf(stderr, "\n%s:%d in %s ERROR:\n  %s\n", file, line, function, message);
	*((volatile int*)NULL) = 0;
	exit(1);
}

void warning(const char* file, int line, const char* function, const char* message)
{
	fprintf(stderr, "\n%s:%d in %s WARNING:\n  %s\n", file, line, function, message);
}

void abort(const char* file, int line, const char* function)
{
	fprintf(stderr, "\n%s:%d in %s ABORTED.\n", file, line, function);
	*((volatile int*)NULL) = 0;
	exit(1);
}

void abort_handler(int /*sigval*/)
{
	*((volatile int*)NULL) = 0;
	exit(1);
}
