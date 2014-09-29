#ifndef GUARD_RECURSIVE_DIRECTORY_ITERATOR_H
#define GUARD_RECURSIVE_DIRECTORY_ITERATOR_H 1

#include <iterator>
#include <stack>
#include "file_status.h"
#include "directory_iterator.h"

namespace filesystem {
inline namespace v1 {

class recursive_directory_iterator
  : public std::iterator<std::input_iterator_tag, directory_entry>
{
 public:
	// constructors and destructor
	recursive_directory_iterator() noexcept;

	explicit recursive_directory_iterator(const path & p);

	recursive_directory_iterator(const path & p, directory_options options);

	recursive_directory_iterator(const path & p, directory_options options,
	                             std::error_code & ec) noexcept;

	recursive_directory_iterator(const path & p, std::error_code & ec) noexcept;

	recursive_directory_iterator(const recursive_directory_iterator & rhs);

	recursive_directory_iterator(recursive_directory_iterator && rhs) noexcept;

	~recursive_directory_iterator();

	// modifiers
	recursive_directory_iterator &
	  operator = (const recursive_directory_iterator & )
	{
		return *this;
	}

	recursive_directory_iterator &
	  operator = (recursive_directory_iterator && ) noexcept
	{
		return *this;
	}

	recursive_directory_iterator & operator++()
	{
		std::error_code ec;
		increment(ec);
//		if (ec) printf("##==> %s\n", m_current_path.c_str());
		if (ec) throw filesystem_error("Could not advance directory cursor",
		                                m_current_path, ec);
		return *this;
	}

	recursive_directory_iterator & increment(std::error_code & ec) noexcept;

	void pop();

	void disable_recursion_pending();

	// observers
	directory_options options() const;

	int depth() const;

	bool recursion_pending() const;

	const directory_entry & operator * () const
		{ return m_entry; }

	const directory_entry * operator -> () const
		{ return &m_entry; }

	bool operator == (const recursive_directory_iterator & other) const
	{
		return (m_entry.path().empty() && other.m_entry.path().empty());
	}

	bool operator != (const recursive_directory_iterator & other) const
		{ return !(*this == other); }

 private:
	void delegate_construction(std::error_code & ec);
	std::error_code push_state();
	std::error_code do_recursive_open(const path & p);

	void set_to_end_iterator()
	{
		m_handle.reset(nullptr);
		memset(&m_buffer, 0, sizeof(m_buffer));
		m_pathname.clear();
		m_current_path.clear();
		m_depth = 0;
		m_entry.assign(path());
	}

	// Simple struct for keeping state
	struct saved_iterator_state
	{
		saved_iterator_state(const path & p, long value)
		  : directory(p), tellptr(value) { }

		saved_iterator_state(const saved_iterator_state & other) = default;

		saved_iterator_state(saved_iterator_state && other) = default;

		saved_iterator_state &
		operator = (const saved_iterator_state & other) = default;

		saved_iterator_state &
		operator = (saved_iterator_state && other) = default;

		path directory;
		long tellptr;
	};

	std::unique_ptr<DIR, DirCloseFunctor> m_handle;
	struct dirent                         m_buffer;
	directory_options                     m_options;
	path                                  m_pathname;
	path                                  m_current_path;
	unsigned int                          m_depth;
	directory_entry                       m_entry;

	std::stack<saved_iterator_state>      m_stack;
};

inline
recursive_directory_iterator begin(recursive_directory_iterator iter) noexcept
	{ return iter; }

inline
recursive_directory_iterator end(const recursive_directory_iterator &) noexcept
	{ return recursive_directory_iterator(); }

} // inline namespace v1
} // namespace filesystem

#endif // GUARD_RECURSIVE_DIRECTORY_ITERATOR_H
