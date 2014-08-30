#include "exceptionTrace.h"

#include <execinfo.h>
#include <dlfcn.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>

#include <cxxabi.h>
#include <memory>

//
// All of the funky stuff in here is only compiled if NDEBUG is undefined
//
#ifndef NDEBUG
//
// Per-thread trace buffer. This *should* be thread-safe, but this should
// be audited in the future and/or unit tested
//
static __thread struct BacktraceInfo btInfo;

//
// These declarations must appear in this namespace, as they wrap a
// function in the c++ runtime (__cxa_throw) so as to add a hook
// that can be called when a throw statement is executed.
//
namespace __cxxabiv1 {

//
// parameter type for __cxa_throw
//
typedef void (*throwcb_t) (void *);

//
// __cxa_throw function signature
//
typedef void (*throwFuncType)(void *, std::type_info *, throwcb_t)
  __attribute__((noreturn));

//
// This wraps the internal c++ abi function called to throw an exception
//
extern "C"
__attribute__((noreturn))
void __cxa_throw(void * except, std::type_info * data, throwcb_t func)
{
	static throwFuncType realFunction = nullptr;
	if (!realFunction)
	{
		// Implementation Note: This call to dlsym will allocate a small
		// chunk of memory that valgrind will report. Having looked at the
		// code in glibc, it is a one-time allocation that gets freed on
		// the next dlsym call, so not really a leak, just poor implementation
		realFunction = reinterpret_cast<throwFuncType>(
		                 dlsym(RTLD_NEXT, "__cxa_throw"));
		if ( ! realFunction )
		{
			const char * err = dlerror();
			fputs("Error, could not find real __cxa_throw function: ", stderr);
			if (err)
			{
				fputs(dlerror(), stderr);
				fputc('\n', stderr);
			} else
				fputs("unknown error\n", stderr);
			abort();
		}
	}

	btInfo.numFrames = 0;
	btInfo.numFrames = backtrace(btInfo.addressList, btInfo.maxFrames);

	/* noreturn */ realFunction(except, data, func);
}

} // namespace

#endif // NDEBUG

void dumpBacktrace(FILE * outstr, BacktraceInfo * bt)
{
	Dl_info dlinfo;

	fputs("__Backtrace_____________________________"
	      "________________________________________\n"
	      "[ ", outstr);
	for (int i = 0; i < bt->numFrames; ++i)
	{
		void * addr = bt->addressList[i];
		if (dladdr(addr, &dlinfo) && (dlinfo.dli_saddr))
		{
			int status = -1;
			const char * name = nullptr;
			char * demangled = nullptr;
			long offset = reinterpret_cast<long>(addr)
			              - reinterpret_cast<long>(dlinfo.dli_saddr);

			demangled = abi::__cxa_demangle(dlinfo.dli_sname,
			                                nullptr, 0, &status);
			name = (status == 0) ? demangled : dlinfo.dli_sname;
			fprintf(outstr, "at %s[%p] %s[+0x%0lx]",
			        dlinfo.dli_fname, addr, name, offset);

			if (demangled) free(demangled);
		} else
			fprintf(outstr, "at <unknown>[%p]", addr);
		
		if ((i + 1) != bt->numFrames)
			fputs("\n  ", outstr);
		else
			fputc('\n', outstr);
	}
	fputs("] --------------------------------------"
	      "----------------------------------------\n",
	      outstr);
}

void printExceptionInfo(FILE * outstr)
{
	if (! std::current_exception())
		return;

	std::type_info * tip = abi::__cxa_current_exception_type();

	if (tip)
	{
		const char * name = tip->name();
		int status = -1;
		std::unique_ptr<char, void(*)(void*)> demangled {
		  abi::__cxa_demangle(name, 0, 0, &status), std::free};

		fputs("Exception Type: '", outstr);
		fputs(((status == 0) ? demangled.get() : name), outstr);
		fputs("'\n", outstr);

		try {
			throw;
		} catch (const std::exception & e)
		{
			fputs(" -> what(): ", outstr);
			fputs(e.what(), outstr);
			fputc('\n', outstr);
		} catch (...)
		{
			fputs(" -> No supplemental information available\n", outstr);
		}
	}
#ifndef NDEBUG
	dumpBacktrace(outstr, &btInfo);
#endif
}

