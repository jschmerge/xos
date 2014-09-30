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
  , m_entry()
  , m_stack()
  , m_recurse_flag(true)
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
  , m_entry()
  , m_stack()
  , m_recurse_flag(true)
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
  , m_entry()
  , m_stack()
  , m_recurse_flag(true)
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

recursive_directory_iterator::
recursive_directory_iterator(const recursive_directory_iterator & other)
  : m_handle(nullptr)
  , m_buffer()
  , m_options(other.m_options)
  , m_pathname(other.m_pathname)
  , m_current_path(other.m_current_path)
  , m_entry(other.m_entry)
  , m_stack(other.m_stack)
  , m_recurse_flag(other.m_recurse_flag)
{
	if (other != recursive_directory_iterator())
	{
		memcpy(&m_buffer, &other.m_buffer, sizeof(m_buffer));
		m_handle.reset(opendir(m_current_path.c_str()));

		if (m_handle)
		{
			long tellptr = telldir(other.m_handle.get());
			if (tellptr != -1)
				seekdir(m_handle.get(), tellptr);
			else
				throw filesystem_error("Could not seekdir() on iterator copy",
				                       m_current_path, make_errno_ec());
		} else
		{
			throw filesystem_error("Could not opendir() on iterator copy",
			                       m_current_path, make_errno_ec());
		}
	} else
	{
		set_to_end_iterator();
	}
}

recursive_directory_iterator::
recursive_directory_iterator(recursive_directory_iterator && other) noexcept
  : m_handle(std::move(other.m_handle))
  , m_buffer()
  , m_options(other.m_options)
  , m_pathname(std::move(other.m_pathname))
  , m_current_path(std::move(other.m_current_path))
  , m_entry(std::move(other.m_entry))
  , m_stack(std::move(other.m_stack))
  , m_recurse_flag(other.m_recurse_flag)
{
	memcpy(&m_buffer, &other.m_buffer, sizeof(m_buffer));
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

std::error_code recursive_directory_iterator::push_state()
{
	std::error_code ec;
	long location = telldir(m_handle.get());

	if (location == -1)
		ec = make_errno_ec();
	else
		m_stack.emplace_back(m_current_path, telldir(m_handle.get()));
	return ec;
}

void recursive_directory_iterator::pop()
{
	m_handle.reset(opendir(m_stack.back().directory.c_str()));
	if (! m_handle)
		throw filesystem_error("Could not re-open directory",
		                       m_stack.back().directory.c_str(),
		                       make_errno_ec());

	seekdir(m_handle.get(), m_stack.back().tellptr);
	m_current_path = m_stack.back().directory;
	m_entry.assign(m_current_path / "/");
	m_stack.pop_back();
}

std::error_code recursive_directory_iterator::do_recursive_open(const path & p)
{
	std::error_code ec;

	push_state();
	m_current_path = p;
	m_handle.reset(opendir(m_current_path.c_str()));

	if (! m_handle)
	{
		ec = make_errno_ec();
		m_handle.reset();
		m_pathname.clear();
	} else
	{
		m_current_path /= "/";
		m_entry.assign(m_current_path);
	}

	return ec;
}

bool recursive_directory_iterator::recursion_pending() const
{
	bool rc = false;

	if ( (!m_entry.path().empty()) && (!is_linking_directory(m_entry)) )
	{
		if ( (m_options & directory_options::follow_directory_symlink)
		        == directory_options::none)
		{
			rc = (m_entry.symlink_status().type() == file_type::directory);
		} else
		{
			rc = (m_entry.symlink_status().type() == file_type::directory);
		}
	}

	return rc;
}

recursive_directory_iterator &
recursive_directory_iterator::increment(std::error_code & ec) noexcept
{
	struct dirent * de;
	int rv = 0;
	ec.clear();

	if (! m_handle)
	{
		set_to_end_iterator();
		ec = std::make_error_code(std::errc::bad_file_descriptor);
		return *this;
	}

	if (!m_entry.path().empty() && recursion_pending())
	{
		if ((ec = do_recursive_open(m_entry)))
		{
			if (  ( (ec.value() == EACCES) || (ec.value() == EPERM) )
			   && ( (m_options & directory_options::skip_permission_denied)
			            != directory_options::none) )
			{
				// error opening isn't problem with skip directories enabled
				ec.clear();
				pop();
			} else
			{
				path tmp = m_entry;
				set_to_end_iterator();
				m_current_path = tmp;
				return *this;
			}
		}
	}

	while (  ((rv = readdir_r(m_handle.get(), &m_buffer, &de)) == 0)
	      && (de == nullptr) && !m_stack.empty() )
	{
		pop();
	}

	if (rv == 0 && de == nullptr && m_stack.empty())
	{
		set_to_end_iterator();
	} else if (  ((m_options & directory_options::skip_permission_denied)
	                != directory_options::none)
		      && ((rv == EPERM) || (rv == EACCES)) )
	{
		// error opening isn't problem with skip directories enabled
		ec.clear();
		pop();
	} else if (rv != 0)
	{
		ec = make_errno_ec(rv);
	} else
	{
		path tmp(m_current_path);
		tmp /= m_buffer.d_name;
		m_entry.assign(tmp);
	}

	return *this;
}

} // inline namespace v1
} // namespace filesystem
