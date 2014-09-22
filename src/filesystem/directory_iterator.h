#ifndef GUARD_DIRECTORY_ITERATOR_H
#define GUARD_DIRECTORY_ITERATOR_H 1

#include <dirent.h>

#include <system_error>
#include <iterator>

#include "../utility/util.h"
#include "../utility/bitmask_operators.h"

#include "path.h"

namespace filesystem {
inline namespace v1 {

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
	typedef ptrdiff_t               difference_type;
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

	~directory_iterator();

	directory_iterator & operator = (const directory_iterator & rhs);

	directory_iterator & operator = (directory_iterator && rhs) noexcept;

	const directory_entry & operator * () const;

	const directory_entry * operator -> () const;

	directory_iterator & operator ++ ();

	directory_iterator & increment(std::error_code & ec) noexcept;

 private:
	std::unique_ptr<DIR, DirCloseFunctor> handle;
	struct dirent buffer;
	directory_options m_options;
};

directory_iterator begin(directory_iterator iter) noexcept;

directory_iterator end(const directory_iterator &) noexcept;

} // inline namespace v1
} // namespace filesystem

#endif // GUARD_DIRECTORY_ITERATOR_H
