#include "recursive_directory_iterator.h"
#include "directory_iterator.h"
#include "path.h"

namespace filesystem {
inline namespace v1 {

recursive_directory_iterator::recursive_directory_iterator() noexcept
  : m_options(directory_options::none)
  , m_pathname()
  , m_entry()
	{ }

recursive_directory_iterator::recursive_directory_iterator(const path & p)
  : m_options(directory_options::none)
  , m_pathname(p)
  , m_entry()
{
}

recursive_directory_iterator::recursive_directory_iterator(
	const path & p, directory_options options)
  : m_options(options)
  , m_pathname(p)
  , m_entry()
{
}

recursive_directory_iterator::recursive_directory_iterator(
	const path & p, directory_options options, std::error_code & ec) noexcept
  : m_options(options)
  , m_pathname(p)
  , m_entry()
{
}

recursive_directory_iterator::recursive_directory_iterator(
	const path & p, std::error_code & ec) noexcept
  : m_options(directory_options::none)
  , m_pathname(p)
  , m_entry()
{
}

recursive_directory_iterator::recursive_directory_iterator(
	const recursive_directory_iterator & rhs)
  : m_options(rhs.m_options)
  , m_pathname(rhs.m_pathname)
  , m_entry()
{
}

recursive_directory_iterator::recursive_directory_iterator(
	recursive_directory_iterator && rhs) noexcept
  : m_options(rhs.m_options)
  , m_pathname(std::move(rhs.m_pathname))
  , m_entry()
{
}

recursive_directory_iterator::~recursive_directory_iterator()
	{ }

} // inline namespace v1
} // namespace filesystem
