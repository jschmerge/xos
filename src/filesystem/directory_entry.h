#ifndef GUARD_DIRECTORY_ENTRY_H
#define GUARD_DIRECTORY_ENTRY_H 1

#include <system_error>

namespace filesystem {
inline namespace v1 {

class file_status;
class path;

class directory_entry
{
 public:
	directory_entry() noexcept                                 = default;
	directory_entry(const directory_entry &)                   = default;
	directory_entry(directory_entry &&) noexcept               = default;
	directory_entry & operator = (const directory_entry &)     = default;
	directory_entry & operator = (directory_entry &&) noexcept = default;

	explicit directory_entry(const path& p);

	~directory_entry();

	void assign(const path & p);

	void replace_filename(const path & p);

	const path& path() const noexcept;

	operator const class path &() const noexcept;

	file_status status() const;
	file_status status(std::error_code & ec) const noexcept;

	file_status symlink_status() const;
	file_status symlink_status(std::error_code & ec) const noexcept;

	bool operator == (const directory_entry & rhs) const noexcept;
	bool operator != (const directory_entry & rhs) const noexcept;

	bool operator < (const directory_entry & rhs) const noexcept;
	bool operator <= (const directory_entry & rhs) const noexcept;

	bool operator > (const directory_entry & rhs) const noexcept;
	bool operator >= (const directory_entry & rhs) const noexcept;
};

} // inline namespace v1
} // namespace filesystem

#endif // GUARD_DIRECTORY_ENTRY_H
