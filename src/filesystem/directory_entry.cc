#include "directory_entry.h"


namespace filesystem {
inline namespace v1 {

directory_entry::directory_entry(const directory_entry::path_entry & p)
  : pathname(p)
	{ }

directory_entry::~directory_entry() { }

void directory_entry::assign(const path_entry & p)
{
	pathname = p;
}

void directory_entry::replace_filename(const path_entry & p)
{
	pathname.replace_filename(p);
}

} // inline namespace v1
} // namespace filesystem
