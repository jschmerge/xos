#ifndef GUARD_EXCEPTIONTRACE_H
#define GUARD_EXCEPTIONTRACE_H 1

#include <cstdio>

#ifndef MAXFRAMES
#define MAXFRAMES 64
#endif

///
/// Contains the backtrace info we use from the throw site in a thread
/// Future work might include making this more of a class-with functions and
/// exposing it to the wold.
///
struct BacktraceInfo {
	static const int maxFrames = MAXFRAMES;
	int numFrames;
	void * addressList[maxFrames];
};

///
/// Prints a backtrace, stored in a BacktraceInfo struct to the stream
/// specified
///
void dumpBacktrace(FILE * outstr, BacktraceInfo * bt);

///
/// User callable function that provides as much info as can be garnered from
/// an exception under normal circumstances. This is meant to be used primarily
/// for debugging, but also as a last-stop catch(...) block body. Instead
/// of catching the program's termination handler.
///
/// If program is compiled without NDEBUG defined, this will print a stack
/// trace for the thrown exception
///
void printExceptionInfo(FILE * outstr);

#endif // GUARD_EXCEPTIONTRACE_H
