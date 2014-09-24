#include "directory_iterator.h"
#include "filesystem_error.h"

#include <cstring>

namespace filesystem {
inline namespace v1 {

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
	if (m_pathname.empty())
		throw filesystem_error("Empty directory pathname",
		                       std::make_error_code(
		                         std::errc::invalid_argument));

	m_handle.reset(opendir(m_pathname.c_str()));

	if (! m_handle)
		throw filesystem_error("Could not open directory",
		                        p, make_errno_ec());

	m_pathname /= "/";
	m_entry.assign(m_pathname);

	std::error_code ec;
	increment(ec);

	if (ec)
		throw filesystem_error("Could not read directory entry", p, ec);
#if 0
	if ( ! m_handle)
		throw filesystem_error("Could not open directory",
		                       p, make_errno_ec());

	std::error_code ec;

	m_pathname /= '/';

	m_entry.assign(m_pathname);

	increment(ec);

	if (ec)
		throw filesystem_error("Could not read directory entry",
		                       p, ec);
#endif
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
			throw filesystem_error("Empty directory pathname",
			                       std::make_error_code(
			                         std::errc::invalid_argument));
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
	return *this;
}


directory_iterator &
directory_iterator::operator = (directory_iterator && other) noexcept
{
	if (this != &other)
	{
		m_handle = std::move(other.m_handle);
		m_buffer = std::move(other.m_buffer);
		m_options = other.m_options;
		m_pathname = std::move(other.m_pathname);
		m_entry.assign(m_pathname);

		rewinddir(m_handle.get());
		std::error_code ec;
		increment(ec);
	}
	return *this;
}

directory_iterator &
directory_iterator::increment(std::error_code & ec) noexcept
{
	struct dirent * de;
	static_assert(std::is_same<decltype(m_handle.get()), DIR*>::value, "");
	int rv = 0;

	if (! m_handle)
	{
		ec = std::make_error_code(std::errc::bad_file_descriptor);

	} else if ((rv = readdir_r(m_handle.get(), &m_buffer, &de)) == 0)
	{
		if (de == nullptr)
			m_entry.assign(path());
		else
			m_entry.replace_filename(m_buffer.d_name);
	} else
	{
		ec = std::error_code(rv, std::system_category());
	}
	return *this;
}

} // inline namespace v1
} // namespace filesystem
