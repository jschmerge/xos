#ifndef GUARD_DIRECTORY_ITERATOR_H
#define GUARD_DIRECTORY_ITERATOR_H 1

#include <dirent.h>

#include <system_error>
#include <iterator>

#include "../utility/util.h"
#include "../utility/bitmask_operators.h"

#include "path.h"
#include "directory_entry.h"

namespace filesystem {
inline namespace v1 {

class path;
class directory_entry;

enum class directory_options
{
	none                     = 0,
	follow_directory_symlink = 1,
	skip_permission_denied   = 2,
};

DEFINE_BITMASK_OPERATORS(directory_options, unsigned int);

class directory_iterator
  : public std::iterator<std::forward_iterator_tag, directory_entry>
{
 public:
	// member functions
	directory_iterator() noexcept;

	explicit directory_iterator(const path & p);

	directory_iterator(const path & p, directory_options options);

	directory_iterator(const path & p, std::error_code & ec) noexcept;

	directory_iterator(const path & p, directory_options options,
	                   std::error_code & ec) noexcept;

	directory_iterator(const directory_iterator & rhs);

	directory_iterator(directory_iterator && rhs) noexcept;

	~directory_iterator();

	directory_iterator & operator = (const directory_iterator & rhs);

	directory_iterator & operator = (directory_iterator && rhs) noexcept;

	const directory_entry & operator * () const
		{ return m_entry; }

	const directory_entry * operator -> () const
		{ return &m_entry; }

	directory_iterator & operator ++ ()
	{
		std::error_code ec;
		increment(ec);
		if (ec) throw filesystem_error("Could not advance directory cursor",
		                               m_pathname, ec);
		return *this;
	}

	directory_iterator & increment(std::error_code & ec) noexcept;

	bool operator == (const directory_iterator & other) const
	{
		return (m_entry.path().empty() && other.m_entry.path().empty());
	}

	bool operator != (const directory_iterator & other) const
		{ return !(*this == other); }

 private:
	std::unique_ptr<DIR, DirCloseFunctor> m_handle;
	struct dirent                         m_buffer;
	directory_options                     m_options;
	path                                  m_pathname;
	directory_entry                       m_entry;
};

inline directory_iterator begin(directory_iterator iter) noexcept
	{ return iter; }

inline directory_iterator end(const directory_iterator &) noexcept
	{ return directory_iterator(); }

} // inline namespace v1
} // namespace filesystem

#endif // GUARD_DIRECTORY_ITERATOR_H
