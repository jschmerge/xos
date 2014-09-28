#include "recursive_directory_iterator.h"
#include "directory_iterator.h"
#include "path.h"

namespace filesystem {
inline namespace v1 {

recursive_directory_iterator::recursive_directory_iterator() noexcept
  : m_handle(nullptr)
  , m_buffer()
  , m_options(directory_options::none)
  , m_pathname()
  , m_depth(0)
  , m_entry()
	{ }

recursive_directory_iterator::recursive_directory_iterator(
	const path & p, directory_options options)
  : m_handle(nullptr)
  , m_buffer()
  , m_options(options)
  , m_pathname(p)
  , m_depth(0)
  , m_entry()
{
}

recursive_directory_iterator::recursive_directory_iterator(
	const path & p, directory_options options, std::error_code & ec) noexcept
  : m_handle(nullptr)
  , m_buffer()
  , m_options(options)
  , m_pathname(p)
  , m_depth(0)
  , m_entry()
{
}

recursive_directory_iterator::recursive_directory_iterator(
	const path & p, std::error_code & ec) noexcept
  : recursive_directory_iterator(p, directory_options::none, ec)
	{ }

recursive_directory_iterator::recursive_directory_iterator(const path & p)
  : recursive_directory_iterator(p, directory_options::none)
	{ }

recursive_directory_iterator::recursive_directory_iterator(
	const recursive_directory_iterator & other)
  : recursive_directory_iterator(other.m_pathname, other.m_options)
	{ }

recursive_directory_iterator::recursive_directory_iterator(
	recursive_directory_iterator && other) noexcept
{
	// FIXME
}

recursive_directory_iterator::~recursive_directory_iterator()
	{ }

} // inline namespace v1
} // namespace filesystem
