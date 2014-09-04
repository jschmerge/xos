#include "path.h"
#include "filesystem"

namespace filesystem {
inline namespace v1 {

path::path() noexcept
  : pathname()
  , seperators() 
	{ }

path::path(const path & other)
  : pathname(other.pathname)
  , seperators(other.seperators)
	{ }

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

/// The append operations use operator /= to denote their semantic effect
/// of appending preferred-separator when needed.
///
/// Appends path::preferred_separator to pathname unless:
/// - an added separator would be redundant, or
/// - would change a relative path to an absolute path
///    [Note: An empty path is relative. -- end note], or
/// - p.empty() , or
/// - *p.native().cbegin() is a directory separator.
path & path::operator /= (const path & p)
{
	if ( ! p.empty() )
	{
		if (this->empty())
		{
			pathname = p.pathname;
		} else
		{
			if (*pathname.rbegin() != preferred_separator)
				pathname += preferred_separator;

			if (*p.pathname.begin() == preferred_separator)
				pathname += p.pathname.substr(1);
			else
				pathname += p.pathname;
		}
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

path::operator string_type() const
{
	return pathname;
}

bool path::empty() const noexcept
{
	return pathname.empty();
}

} /*v1*/
} /*filesystem*/
