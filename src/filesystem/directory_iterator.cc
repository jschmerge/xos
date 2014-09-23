#include "directory_iterator.h"
#include "filesystem_error.h"

#include <cstring>

namespace filesystem {
inline namespace v1 {

const DirCloseFunctor directory_iterator::directory_closer;

directory_iterator::directory_iterator() noexcept
  : m_handle(nullptr)
  , m_buffer()
  , m_options(directory_options::none)
  , m_entry()
{
	memset(&m_buffer, 0, sizeof(m_buffer));
}

directory_iterator::directory_iterator(const path & p)
  : directory_iterator(p, directory_options::none)
	{ }

directory_iterator::directory_iterator(const path & p,
                                       directory_options options)
  : m_handle(opendir(p.c_str()), directory_closer)
  , m_buffer()
  , m_options(options)
  , m_entry()
{
	memset(&m_buffer, 0, sizeof(m_buffer));

	if ( ! m_handle)
		throw filesystem_error("Could not open directory", p, make_errno_ec());

	std::error_code ec;
	increment(ec);

	if (ec)
		throw filesystem_error("Could not read directory entry", p, ec);
}

directory_iterator::directory_iterator(const path & p,
                                       directory_options options,
	                                   std::error_code & ec) noexcept
  : m_handle(opendir(p.c_str()), directory_closer)
  , m_buffer()
  , m_options(options)
  , m_entry()
{
	ec.clear();
	memset(&m_buffer, 0, sizeof(m_buffer));

	if (m_handle)
		increment(ec);
	else
		ec = make_errno_ec();
}

directory_iterator::directory_iterator(const directory_iterator & other)
	// XXX - we should really think about opening/dup'ing a new handle to
	// the directory here
  : m_handle(other.m_handle)
  , m_buffer(other.m_buffer)
  , m_options(other.m_options)
  , m_entry(other.m_entry)
	{ }

directory_iterator::directory_iterator(directory_iterator && other) noexcept
  : m_handle(std::move(other.m_handle))
  , m_buffer(std::move(other.m_buffer))
  , m_options(std::move(other.m_options))
  , m_entry(std::move(other.m_entry))
	{ }

directory_iterator &
directory_iterator::increment(std::error_code & ec) noexcept
{
	struct dirent * rc;
	static_assert(std::is_same<decltype(m_handle.get()), DIR*>::value, "");

	if (readdir_r(m_handle.get(), &m_buffer, &rc) == 0)
	{
		if (rc == nullptr)
			m_entry.assign(path());
		else
			m_entry.replace_filename(m_buffer.d_name);
	} else
	{
		ec = make_errno_ec();
	}
	return *this;
}

} // inline namespace v1
} // namespace filesystem
