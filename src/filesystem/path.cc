
#include <cstring> // for strcmp

#include "path.h"
#include "filesystem_error.h"

namespace {
	
} // namespace

namespace filesystem {
inline namespace v1 {

constexpr path::value_type path::preferred_separator;


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
	bool modified = false;

	while (!pathname.empty() && (*pathname.rbegin() == preferred_separator))
	{
		pathname.pop_back();
		modified = true;
	}

	while (  ! modified
	      && ! pathname.empty()
	      && *pathname.rbegin() != preferred_separator)
	{
		pathname.pop_back();
	}

	while (  pathname.length() > 1
	      && (*pathname.rbegin() == preferred_separator))
	{
		pathname.pop_back();
	}

	return *this;
}

path & path::replace_filename(const path & replacement)
{
	remove_filename() /= replacement;
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

std::string path::string() const
{
	return pathname;
}

std::wstring path::wstring() const
{
	assert(0);
	return L"";
}

std::string path::u8string() const
{
	return pathname;
}

std::u16string path::u16string() const
{
	assert(0);
	return u"";
}

std::u32string path::u32string() const
{
	assert(0);
	return U"";
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


//////////////////////////////////////////////////////////////////////
// decomposition
// path path::root_path() const;

path path::relative_path() const
{
	path ret;
	if (pathname[0] == preferred_separator)
	{
		string_type::size_type pos
		  = pathname.find_first_not_of(preferred_separator);

		ret = pathname.substr(pos, string_type::npos);
	}
	return ret;
}

path path::root_name() const
{
	return path();
}

path path::root_directory() const
{
	path ret;
	if (has_root_directory())
		ret += preferred_separator;

	return ret;
}

path path::parent_path() const
{
	path ret;

	if (!empty())
	{
		for (auto i = begin(); i != --end(); ++i)
			ret /= *i;
	}

	return ret;
}

path path::filename() const
{
	return empty() ? path() : *--end();
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
	return (pathname[0] == preferred_separator);
}

bool path::has_root_path() const
{
	return false;
}

bool path::has_relative_path() const
{
	return ( ! has_root_directory());
}

bool path::has_parent_path() const
{
	return ( ! parent_path().empty() );
}

bool path::has_filename() const
{
	return !filename().empty();
}

bool path::has_stem() const
{
	return !stem().empty();
}

bool path::has_extension() const
{
	return !extension().empty();
}

bool path::is_absolute() const
{
	return (*pathname.begin() == preferred_separator);
}

bool path::is_relative() const
{
	return (*pathname.begin() != preferred_separator);
}

path path::stem() const
{
	std::string s = filename();
	path st(s);
	std::string::size_type n = 0;

	if (  ( ! is_linking_directory(*this) )
	   && ( (n = s.find_last_of('.')) != std::string::npos ) )
		st = s.substr(0, n);

	return st;
}

path path::extension() const
{
	path ext;
	std::string s = filename();
	std::string::size_type n = 0;

	if (  ( ! is_linking_directory(*this) )
	   && ( (n = s.find_last_of('.')) != std::string::npos ) )
		ext = s.substr(n);

	return ext;
}

} /*v1*/
} /*filesystem*/
