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
	none = 0,
	follow_directory_symlink = 1,
	skip_permission_denied = 2,
};

DEFINE_BITMASK_OPERATORS(directory_options, unsigned int);

class directory_iterator
{
public:
	typedef directory_entry         value_type;
	typedef std::ptrdiff_t          difference_type;
	typedef const directory_entry * pointer;
	typedef const directory_entry & reference;
	typedef std::input_iterator_tag iterator_category;

	// member functions
	directory_iterator() noexcept;

	explicit directory_iterator(const path & p);

	directory_iterator(const path & p, directory_options options);

	directory_iterator(const path & p, std::error_code & ec) noexcept;

	directory_iterator(const path & p, directory_options options,
	                   std::error_code & ec) noexcept;

	directory_iterator(const directory_iterator & rhs);

	directory_iterator(directory_iterator && rhs) noexcept;

	~directory_iterator() { }

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
		// XXX - include directory path in exception
		if (ec) throw filesystem_error("Could not advance directory cursor",
		                               ec);

		return *this;
	}

	directory_iterator & increment(std::error_code & ec) noexcept;

	bool operator == (const directory_iterator & other) const
	{
		if (m_entry.path().empty() && other.m_entry.path().empty())
			return true;
		else
		{
			// FIXME
		}
		return false;
	}

	bool operator != (const directory_iterator & other) const
		{ return !(*this == other); }

 private:
	static const DirCloseFunctor directory_closer;

	std::shared_ptr<DIR> m_handle;
	struct dirent        m_buffer;
	directory_options    m_options;
	directory_entry      m_entry;

//	void readEntry(std::error_code & ec);
};

inline directory_iterator begin(directory_iterator iter) noexcept
	{ return iter; }

inline directory_iterator end(const directory_iterator &) noexcept
	{ return directory_iterator(); }

} // inline namespace v1
} // namespace filesystem

#endif // GUARD_DIRECTORY_ITERATOR_H
