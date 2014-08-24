#include "descriptor.h"

#include <unistd.h>
#include <fcntl.h>

#include <cassert>
#include <stdexcept>

namespace io {

bad_close_handler_t *
descriptor::bad_close_handler = default_bad_close_handler;

//////////////////////////////////////////////////////////////////////
void default_bad_close_handler(int fd, std::exception * ex) noexcept
{
	if (ex != nullptr)
	{
		fprintf(stderr, "Error: close for descriptor %d failed: %s; aborting\n",
		        fd, ex->what());
	} else
	{
		fprintf(stderr, "Error: close for descriptor %d failed; aborting\n",
		        fd);
	}

	std::terminate();
}

//////////////////////////////////////////////////////////////////////
bad_close_handler_t *
descriptor::set_bad_close_handler(bad_close_handler_t * new_handler)
{
	bad_close_handler_t * old_handler{bad_close_handler};

	if (new_handler != nullptr)
		bad_close_handler = new_handler;

	return old_handler;
}

//////////////////////////////////////////////////////////////////////
descriptor::descriptor(descriptor && other)
{
	std::swap(fd, other.fd);
}

//////////////////////////////////////////////////////////////////////
descriptor & descriptor::operator = (descriptor && other)
{
	if (this != &other)
		std::swap(fd, other.fd);

	return *this;
}

//////////////////////////////////////////////////////////////////////
descriptor::~descriptor()
{
	assert(bad_close_handler != nullptr);

	try                        { this->close(); }
	catch (std::exception & e) { bad_close_handler(fd, &e); }
	catch (...)                { bad_close_handler(fd, nullptr); }
}

//////////////////////////////////////////////////////////////////////
void descriptor::close()
{
	if (fd >= 0)
	{
		if (::close(fd) != 0)
			throw make_syserr("descriptor close failed");

		fd = -1;
	}
}

//////////////////////////////////////////////////////////////////////
descriptor descriptor::dup(int newFd, bool close_on_exec)
{
	int copyFd = -1;

	if (newFd < 0)
		copyFd = ::fcntl(fd, close_on_exec ? F_DUPFD_CLOEXEC : F_DUPFD);
	else
		copyFd = ::dup3(fd, newFd, close_on_exec ? O_CLOEXEC : 0);

	if (copyFd < 0)
		throw make_syserr("Could not duplicate descriptor");

	descriptor ret(copyFd);

	return ret;
}

} // namespace io
