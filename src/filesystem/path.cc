
#include <cstring> // for strcmp

#include "path.h"
#include "filesystem_error.h"


namespace filesystem {
inline namespace v1 {

path::path() noexcept : pathname() { }

path::path(const path & other) : pathname(other.pathname) { }

path::path(path && other) noexcept : pathname(std::move(other.pathname)) { }

path::~path() { }

path & path::operator = (const path & other)
{
	if (this != &other)
		pathname = other.pathname;

	return *this;
}

path & path::operator = (path && other) noexcept
{
	using std::swap;

	if (this != &other)
		swap(pathname, other.pathname);

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

path & path::operator += (const path & other)
{
	this->pathname += other.pathname;
	return *this;
}

path & path::operator += (const string_type & other)
{
	this->pathname += other;
	return *this;
}

path & path::operator += (const value_type * other)
{
	for (const value_type * i = other; *i != value_type(); ++i)
		this->pathname += *i;
	return *this;
}

path & path::operator += (value_type other)
{
	this->pathname += other;
	return *this;
}

void path::clear() noexcept
{
	pathname.clear();
}

path & path::make_preferred()
{
	return *this; // no-op on posix
}

path & path::remove_filename()
{
	string_type::size_type i = (pathname.length() - 1);

	if (pathname[i] == preferred_separator)
		pathname.erase(i);

	return *this;
}

void path::swap(path & rhs) noexcept
{
	std::swap(pathname, rhs.pathname);
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

//////////////////////////////////////////////////////////////////////
// compare
int path::compare(const path & p) const noexcept
{
	return strcmp(c_str(), p.c_str());
}

int path::compare(const string_type & s) const
{
	return strcmp(c_str(), s.c_str());
}

int path::compare(const value_type * s) const
{
	return strcmp(c_str(), s);
}

bool path::empty() const noexcept
{
	return pathname.empty();
}

bool path::has_root_name() const
{
	return false;
}

bool path::has_root_directory() const
{
	return false;
}

bool path::has_root_path() const
{
	return false;
}

bool path::has_relative_path() const
{
	return false;
}

bool path::has_parent_path() const
{
	return false;
}

bool path::has_filename() const
{
	return false;
}

bool path::has_stem() const
{
	return false;
}

bool path::has_extension() const
{
	return false;
}

bool path::is_absolute() const
{
	return (*pathname.begin() == preferred_separator);
}

bool path::is_relative() const
{
	return (*pathname.begin() != preferred_separator);
}

} /*v1*/
} /*filesystem*/
