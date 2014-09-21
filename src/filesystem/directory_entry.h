#ifndef GUARD_DIRECTORY_ENTRY_H
#define GUARD_DIRECTORY_ENTRY_H 1

#include <system_error>

#include "path.h"

namespace filesystem {
inline namespace v1 {

class file_status;

class directory_entry
{
 public:
	// For disambiguating the path() member function from the class
	typedef path path_entry;

	directory_entry() noexcept                                 = default;
	directory_entry(const directory_entry &)                   = default;
	directory_entry(directory_entry &&) noexcept               = default;
	directory_entry & operator = (const directory_entry &)     = default;
	directory_entry & operator = (directory_entry &&) noexcept = default;

	explicit directory_entry(const path_entry & p);

	~directory_entry();

	void assign(const path_entry & p);

	void replace_filename(const path_entry & p);

	// XXX - 'path' as the function name here doesn't compile with gcc 4.8.x
	// ...clang doesn't seem to have problems with it however
	const filesystem::v1::path & _path() const noexcept
		{ return pathname; }

	operator const path_entry & () const noexcept
		{ return pathname; }

	// XXX
	file_status status() const;
	file_status status(std::error_code & ec) const noexcept;

	// XXX
	file_status symlink_status() const;
	file_status symlink_status(std::error_code & ec) const noexcept;

	bool operator == (const directory_entry & rhs) const noexcept
		{ return pathname == rhs.pathname; }
	bool operator != (const directory_entry & rhs) const noexcept
		{ return pathname != rhs.pathname; }
	bool operator < (const directory_entry & rhs) const noexcept
		{ return pathname < rhs.pathname; }
	bool operator <= (const directory_entry & rhs) const noexcept
		{ return pathname <= rhs.pathname; }
	bool operator > (const directory_entry & rhs) const noexcept
		{ return pathname > rhs.pathname; }
	bool operator >= (const directory_entry & rhs) const noexcept
		{ return pathname >= rhs.pathname; }

 private:
	path_entry pathname;
};

} // inline namespace v1
} // namespace filesystem

#endif // GUARD_DIRECTORY_ENTRY_H
