#include "directory_iterator.h"

#include <cstring>

namespace filesystem {
inline namespace v1 {

directory_iterator::directory_iterator() noexcept
  : handle(nullptr)
  , buffer()
  , m_options(directory_options::none)
	{ memset(&buffer, 0, sizeof(buffer)); }

directory_iterator::directory_iterator(const path & p)
  : handle(opendir(p.c_str()))
  , buffer()
  , m_options(directory_options::none)
{
	memset(&buffer, 0, sizeof(buffer));
}

directory_iterator::directory_iterator(const path & p,
                                       directory_options options)
  : handle(opendir(p.c_str()))
  , buffer()
  , m_options(options)
{
	memset(&buffer, 0, sizeof(buffer));
}

directory_iterator::directory_iterator(const path & p,
                                       directory_options options,
	                                   std::error_code & ec) noexcept
  : handle(opendir(p.c_str()))
  , buffer()
  , m_options(options)
{
	memset(&buffer, 0, sizeof(buffer));
}

} // inline namespace v1
} // namespace filesystem
