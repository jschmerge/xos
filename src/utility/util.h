#ifndef GUARD_UTIL_H
#define GUARD_UTIL_H 1

#include <dirent.h>

#include <type_traits>
#include <string>
#include <memory>
#include <system_error>


///
/// Simple functor for freeing malloc-allocated memory
///
struct MallocDeleter
{
	MallocDeleter() { }
	void operator () (void * p) const { free(p); }
};
 
///
/// Simple functor for closing a C stdio FILE*
///
struct FCloseFunctor
{
	FCloseFunctor() { }
	void operator () (FILE * f) const { fclose(f); }
};

///
/// Simple functor for closing a C stdio DIR*
///
struct DirCloseFunctor
{
	DirCloseFunctor() { }
	void operator () (DIR * f) const { closedir(f); }
};

///
/// RAII Wrapper for C stdio FILE* objects
///
typedef std::unique_ptr<FILE, FCloseFunctor> FilePtr;

///
/// Convienience functions for making system_error exception
///
inline std::system_error make_syserr(int e, const char * msg)
{
	return std::system_error(std::error_code(e, std::system_category()), msg);
}

inline std::system_error make_syserr(int e, const std::string & msg)
	{ return make_syserr(e, msg.c_str()); }

inline std::system_error make_syserr(const std::string & msg)
	{ return make_syserr(errno, msg); }

inline std::system_error make_syserr(const char * msg)
	{ return make_syserr(errno, msg); }

#endif // GUARD_UTIL_H
