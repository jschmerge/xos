#include "path.h"

namespace std {
 namespace experimental {
  namespace filesystem {
   inline namespace v1 {

path::path() noexcept
  : pathname(), seperators() 
	{ }

path::path(const path & other)
  : pathname(other.pathname), seperators(other.seperators) { }

path::path(path && other) noexcept
  : pathname(std::move(other.pathname))
  , seperators(std::move(other.seperators))
	{ }

path::~path() { }

path & path::operator = (const path & other)
{
	if (this != &other)
	{
		pathname = other.pathname;
		seperators = other.seperators;
	}

	return *this;
}

path & path::operator /= (const path & p)
{
	if (!empty() && !p.empty() &&
	    *p.native().cbegin() != preferred_separator &&
	    *native().crbegin() != preferred_separator)
	{
		pathname += preferred_separator;
		seperators.push_back(pathname.length() - 1);
	}

	auto oldLength = pathname.length();

	pathname += p.pathname;

	for (auto idx : p.seperators)
		seperators.push_back(idx + oldLength);

	return *this;
}

path & path::operator = (path && other) noexcept
{
	using std::swap;
	if (this != &other)
	{
		swap(pathname, other.pathname);
		swap(seperators, other.seperators);
	}

	return *this;
}

void path::clear() noexcept
{
	pathname.clear();
	seperators.clear();
}

path & path::make_preferred()
{
	return *this;
}

void path::swap(path & rhs) noexcept
{
	std::swap(pathname, rhs.pathname);
	std::swap(seperators, rhs.seperators);
}

const path::string_type & path::native() const noexcept
{
	return pathname;
}

const path::value_type * path::c_str() const noexcept
{
	return pathname.c_str();
}

bool path::empty() const noexcept
{
	return pathname.empty();
}


   } /*v1*/
  }/*filesystem*/
 } /*experimental*/
} /*std*/

