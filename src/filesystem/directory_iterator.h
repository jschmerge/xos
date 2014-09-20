#ifndef GUARD_DIRECTORY_ITERATOR_H
#define GUARD_DIRECTORY_ITERATOR_H 1

#include <system_error>
#include <iterator>

#include "path.h"

namespace filesystem {
inline namespace v1 {

// XXX - need bitmask operators
enum class directory_options
{
	none = 0,
	follow_directory_symlink = 1,
	skip_permission_denied = 2,
};

class directory_iterator
{
public:
	typedef directory_entry         value_type;
	typedef ptrdiff_t               difference_type;
	typedef const directory_entry * pointer;
	typedef const directory_entry & reference;
	typedef std::input_iterator_tag iterator_category;

	// member functions
	directory_iterator() noexcept;

	explicit directory_iterator(const path& p);

	directory_iterator(const path& p, directory_options options);

	directory_iterator(const path& p, std::error_code& ec) noexcept;

	directory_iterator(const path& p, directory_options options,
	                   std::error_code& ec) noexcept;

	directory_iterator(const directory_iterator& rhs);

	directory_iterator(directory_iterator&& rhs) noexcept;

	~directory_iterator();

	directory_iterator& operator=(const directory_iterator& rhs);

	directory_iterator& operator=(directory_iterator&& rhs) noexcept;

	const directory_entry& operator*() const;

	const directory_entry* operator->() const;

	directory_iterator& operator++();

	directory_iterator& increment(std::error_code& ec) noexcept;
};

class recursive_directory_iterator;

directory_iterator begin(directory_iterator iter) noexcept;

directory_iterator end(const directory_iterator &) noexcept;

recursive_directory_iterator begin(recursive_directory_iterator iter) noexcept;

recursive_directory_iterator end(const recursive_directory_iterator&) noexcept;

} // inline namespace v1
} // namespace filesystem

#endif // GUARD_DIRECTORY_ITERATOR_H
