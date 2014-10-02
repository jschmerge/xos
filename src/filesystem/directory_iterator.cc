#include "directory_iterator.h"
#include "filesystem_error.h"

#include <cstring>

namespace filesystem {
inline namespace v1 {

// Constructs the end() iterator
directory_iterator::directory_iterator() noexcept
  : m_handle(nullptr)
  , m_buffer()
  , m_options(directory_options::none)
  , m_pathname()
  , m_entry()
	{ }

directory_iterator::directory_iterator(const path & p,
                                       directory_options options)
  : m_handle(nullptr)
  , m_buffer()
  , m_options(options)
  , m_pathname(p)
  , m_entry()
{
	std::error_code ec;
	delegate_construction(ec);
	if (ec)
		throw filesystem_error("Could not read directory entry", p, ec);
}

directory_iterator::directory_iterator(const path & p,
                                       directory_options options,
                                       std::error_code & ec) noexcept
  : m_handle(nullptr)
  , m_buffer()
  , m_options(options)
  , m_pathname(p)
  , m_entry()
{
	delegate_construction(ec);
}

directory_iterator::directory_iterator(const path & p)
  : directory_iterator(p, directory_options::none)
	{ }

directory_iterator::directory_iterator(const path & p,
                                       std::error_code & ec) noexcept
  : directory_iterator(p, directory_options::none, ec)
	{ }

directory_iterator::directory_iterator(const directory_iterator & other)
  : directory_iterator(other.m_pathname, other.m_options)
	{ }

directory_iterator::directory_iterator(directory_iterator && other) noexcept
  : m_handle(std::move(other.m_handle))
  , m_buffer(std::move(other.m_buffer))
  , m_options(std::move(other.m_options))
  , m_pathname(std::move(other.m_pathname))
  , m_entry(std::move(other.m_entry))
	{ }

directory_iterator::~directory_iterator()
	{ }

directory_iterator &
directory_iterator::operator = (const directory_iterator & other)
{
	if (this != &other)
	{
		m_options = other.m_options;

		if (other.m_pathname.empty())
		{
			
			m_handle.reset();
			m_pathname.clear();
			m_entry.assign(path{});
		} else
		{
			m_pathname = other.m_pathname;
			m_handle.reset(opendir(m_pathname.c_str()));

			if (! m_handle)
				throw filesystem_error("Could not open directory",
				                        m_pathname, make_errno_ec());

			m_pathname /= "/";
			m_entry.assign(m_pathname);

			std::error_code ec;
			increment(ec);

			if (ec)
				throw filesystem_error("Could not read directory entry",
				                       m_pathname, ec);
		}

	}
	return *this;
}

directory_iterator &
directory_iterator::operator = (directory_iterator && other) noexcept
{
	using std::swap;
	if (this != &other)
	{
		swap(m_handle, other.m_handle);

		swap(m_buffer, other.m_buffer);

		m_options = other.m_options;

		swap(m_pathname, other.m_pathname);

		m_entry.assign(m_pathname);

		if (m_handle)
		{
			rewinddir(m_handle.get());
			std::error_code ec;
			increment(ec);
		}
	}
	return *this;
}

void directory_iterator::delegate_construction(std::error_code & ec) noexcept
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

	if (ec && ( (ec.value() == EPERM) || (ec.value() == EACCES) )
	    && ( (m_options & directory_options::skip_permission_denied)
	          != directory_options::none) )
	{
		ec.clear();
	}
}

directory_iterator &
directory_iterator::increment(std::error_code & ec) noexcept
{
	struct dirent * de = nullptr;
	int rv = 0;

	if (! m_handle)
	{
		ec = std::make_error_code(std::errc::bad_file_descriptor);

	} else
	{
		do {
			if ((rv = readdir_r(m_handle.get(), &m_buffer, &de)) == 0)
			{
				//printf("#####===> %s\n", m_buffer.d_name);
				if (de == nullptr)
					m_entry.assign(path());
				else
					m_entry.replace_filename(m_buffer.d_name);
			} else
			{
				ec = make_errno_ec(rv);
			}
		} while (  (! ec)
		        && ( is_linking_directory(m_entry))
		        && (! m_entry.path().empty()) );
	}

	return *this;
}

} // inline namespace v1
} // namespace filesystem
