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
	directory_entry() noexcept                                 = default;

	directory_entry(const directory_entry &)                   = default;

	directory_entry(directory_entry &&) noexcept               = default;

	directory_entry & operator = (const directory_entry &)     = default;

	directory_entry & operator = (directory_entry &&) noexcept = default;

	explicit directory_entry(const class path & p);

	~directory_entry();

	void assign(const class path & p);

	void replace_filename(const class path & p);

	const class path & path() const noexcept
		{ return pathname; }

	operator const class path & () const noexcept
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
	class path pathname;
};

} // inline namespace v1
} // namespace filesystem

#endif // GUARD_DIRECTORY_ENTRY_H
