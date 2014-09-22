#ifndef GUARD_RECURSIVE_DIRECTORY_ITERATOR_H
#define GUARD_RECURSIVE_DIRECTORY_ITERATOR_H 1

#include "directory_iterator.h"

namespace filesystem {
inline namespace v1 {

class recursive_directory_iterator
{
public:
	typedef directory_entry         value_type;
	typedef ptrdiff_t               difference_type;
	typedef const directory_entry * pointer;
	typedef const directory_entry & reference;
	typedef std::input_iterator_tag iterator_category;

	// constructors and destructor
	recursive_directory_iterator() noexcept;

	explicit recursive_directory_iterator(const path& p);

	recursive_directory_iterator(const path& p, directory_options options);

	recursive_directory_iterator(const path& p, directory_options options,
	                             std::error_code& ec) noexcept;

	recursive_directory_iterator(const path& p, std::error_code& ec) noexcept;

	recursive_directory_iterator(const recursive_directory_iterator& rhs);

	recursive_directory_iterator(recursive_directory_iterator&& rhs) noexcept;

	~recursive_directory_iterator();

	// observers
	directory_options options() const;

	int depth() const;

	bool recursion_pending() const;

	const directory_entry& operator*() const;

	const directory_entry* operator->() const;

	// modifiers
	recursive_directory_iterator &
	  operator = (const recursive_directory_iterator & rhs);

	recursive_directory_iterator &
	  operator = (recursive_directory_iterator && rhs) noexcept;

	recursive_directory_iterator& operator++();

	recursive_directory_iterator& increment(std::error_code & ec) noexcept;

	void pop();

	void disable_recursion_pending();
};


recursive_directory_iterator begin(recursive_directory_iterator iter) noexcept;

recursive_directory_iterator end(const recursive_directory_iterator&) noexcept;

} // inline namespace v1
} // namespace filesystem

#endif // GUARD_RECURSIVE_DIRECTORY_ITERATOR_H
