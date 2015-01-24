#ifndef GUARD_STRUTIL_H
#define GUARD_STRUTIL_H 1

#include <string>
#include <sstream>

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

#endif // GUARD_STRUTIL_H
