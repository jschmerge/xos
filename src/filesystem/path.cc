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

#if XXX
path & path::operator /= (const path & )
{
	return *this;
}
#endif

const path::value_type * path::c_str() const noexcept
{
	return pathname.c_str();
}

   } /*v1*/
  }/*filesystem*/
 } /*experimental*/
} /*std*/

