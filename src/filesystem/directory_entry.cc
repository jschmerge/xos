#include "directory_entry.h"
#include "fs_operations.h"
#include "path.h"


namespace filesystem {
inline namespace v1 {

directory_entry::directory_entry(const class path & p)
  : pathname(p)
	{ }

directory_entry::~directory_entry()
	{ }

void directory_entry::assign(const class path & p)
{
	pathname = p;
}

void directory_entry::replace_filename(const class path & p)
{
	pathname.replace_filename(p);
}

file_status directory_entry::status() const
{
	return filesystem::status(pathname);
}

file_status directory_entry::status(std::error_code & ec) const noexcept
{
	return filesystem::status(pathname, ec);
}

file_status directory_entry::symlink_status() const
{
	return filesystem::symlink_status(pathname);
}

file_status
directory_entry::symlink_status(std::error_code & ec) const noexcept
{
	return filesystem::symlink_status(pathname, ec);
}


} // inline namespace v1
} // namespace filesystem
