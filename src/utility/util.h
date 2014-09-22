#ifndef GUARD_UTIL_H
#define GUARD_UTIL_H 1

#include <dirent.h>

#include <string>
#include <cstdio>

#include <type_traits>
#include <memory>
#include <sstream>
#include <string>
#include <iterator>
#include <system_error>

///
/// Simple functor for freeing malloc-allocated memory
///
struct MallocDeleter { void operator () (void * p) { free(p); } };
 
///
/// Simple functor for closing a C stdio FILE*
///
struct FCloseFunctor { void operator () (FILE * f) { fclose(f); } };

///
/// Simple functor for closing a C stdio DIR*
///
struct DirCloseFunctor { void operator () (DIR * f) { closedir(f); } };

///
/// RAII Wrapper for C stdio FILE* objects
///
typedef std::unique_ptr<FILE, FCloseFunctor> FilePtr;

///
/// convenience function for those times you need an std::string from a
/// character literal
///                 
inline const std::string S(const char * s) { return std::string(s); }
  
///
/// Generic string -> whatever conversion routine. This version has issues,
/// both with performance and bugs in the C/C++ standard library...
/// Specifically, the underlying deficiencies in strto{l,ll,ul,ull}
///
template<class T>
T stringTo(const std::string & s)
{
	T retVal;
	std::istringstream stream(s);
	stream >> retVal;
	return retVal;
}

///
/// Splits a string into substrings along the characters of delim and puts
/// the result into inserter, an output iterator
///
template<typename OUTPUT_ITER>
int split(const std::string & line, const char * delim, OUTPUT_ITER inserter)
{
	int count = 0;
	std::string::size_type n = 0;
	std::string tmp = line;

	while ((n = tmp.find_first_of(delim, 0)) != std::string::npos)
	{
		*inserter = tmp.substr(0, n);
		++inserter;
		++count;
		tmp = tmp.substr(tmp.find_first_not_of(delim, n + 1));
	}

	if (tmp.length())
	{
		*inserter = tmp;
		++inserter;
		++count;
	}

	return count;
}

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
