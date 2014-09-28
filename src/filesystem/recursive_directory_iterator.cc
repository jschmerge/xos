#include "recursive_directory_iterator.h"
#include "directory_iterator.h"
#include "path.h"

namespace filesystem {
inline namespace v1 {

// Constructs the end() iterator
recursive_directory_iterator::recursive_directory_iterator() noexcept
  : m_handle(nullptr)
  , m_buffer()
  , m_options(directory_options::none)
  , m_pathname()
  , m_current_path()
  , m_depth(0)
  , m_entry()
{
	// not really necessary here
	set_to_end_iterator();
}

recursive_directory_iterator::recursive_directory_iterator(
	const path & p, directory_options options)
  : m_handle(nullptr)
  , m_buffer()
  , m_options(options)
  , m_pathname(p)
  , m_current_path(p)
  , m_depth(0)
  , m_entry()
{
	std::error_code ec;
	delegate_construction(ec);
	if (ec)
		throw filesystem_error("Could not read directory entry", p, ec);
}

recursive_directory_iterator::recursive_directory_iterator(
	const path & p, directory_options options, std::error_code & ec) noexcept
  : m_handle(nullptr)
  , m_buffer()
  , m_options(options)
  , m_pathname(p)
  , m_current_path(p)
  , m_depth(0)
  , m_entry()
{
	delegate_construction(ec);
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
	recursive_directory_iterator && ) noexcept
{
	// FIXME
}

recursive_directory_iterator::~recursive_directory_iterator()
	{ }


void recursive_directory_iterator::delegate_construction(std::error_code & ec)
{
	ec.clear();

	if (m_pathname.empty())
	{
		ec = std::make_error_code(std::errc::invalid_argument);
		m_pathname.clear();
		return;
	}

	m_handle.reset(opendir(m_pathname.c_str()));

	if (! m_handle)
	{
		ec = make_errno_ec();
		m_handle.reset();
		m_pathname.clear();
		return;
	}

	m_pathname /= "/";
	m_entry.assign(m_pathname);
	increment(ec);
}

std::error_code recursive_directory_iterator::do_recursive_open(const path & p)
{
	std::error_code ec;

	// XXX - save old directory state here

	m_current_path = p;
	m_handle.reset(opendir(m_current_path.c_str()));

	if (! m_handle)
	{
		ec = make_errno_ec();
		m_handle.reset();
		m_pathname.clear();
	}
	m_pathname /= "/";
	m_entry.assign(m_pathname);

	return ec;
}

recursive_directory_iterator &
recursive_directory_iterator::increment(std::error_code & ec) noexcept
{
	struct dirent * de;
	int rv = 0;
	ec.clear();

	// Bail on invalid
	if (! m_handle)
	{
		set_to_end_iterator();
		ec = std::make_error_code(std::errc::bad_file_descriptor);
		return *this;
	}

	if (recursion_pending())
	{
		ec = do_recursive_open(m_entry);
	}

	if ((rv = readdir_r(m_handle.get(), &m_buffer, &de)) == 0)
	{
		if (de == nullptr)
			set_to_end_iterator();
		else
			m_entry.replace_filename(m_buffer.d_name);
	} else
		ec = make_errno_ec(rv);

	return *this;
}

} // inline namespace v1
} // namespace filesystem
