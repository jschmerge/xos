#include "directory_iterator.h"
#include "filesystem_error.h"

#include <cstring>

namespace filesystem {
inline namespace v1 {

const DirCloseFunctor directory_iterator::directory_closer;

directory_iterator::directory_iterator() noexcept
  : handle(nullptr)
  , buffer()
  , m_options(directory_options::none)
{
	memset(&buffer, 0, sizeof(buffer));
}

directory_iterator::directory_iterator(const path & p)
  : directory_iterator(p, directory_options::none)
	{ }

directory_iterator::directory_iterator(const path & p,
                                       directory_options options)
  : handle(opendir(p.c_str()))
  , buffer()
  , m_options(options)
{
	if ( ! handle)
		throw filesystem_error("Could not open directory", p, make_errno_ec());

	memset(&buffer, 0, sizeof(buffer));
}

directory_iterator::directory_iterator(const path & p,
                                       directory_options options,
	                                   std::error_code & ec) noexcept
  : handle(opendir(p.c_str()))
  , buffer()
  , m_options(options)
{
	if (handle)
		ec.clear();
	else
		ec = make_errno_ec();

	memset(&buffer, 0, sizeof(buffer));
}

} // inline namespace v1
} // namespace filesystem
