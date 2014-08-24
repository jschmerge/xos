#ifndef GUARD_DESCRIPTOR_H
#define GUARD_DESCRIPTOR_H 1

#include <sys/timerfd.h>
#include <unistd.h>

#include <cstdio>
#include <system_error>

#include "util.h"

namespace io {

class fs_path;

//////////////////////////////////////////////////////////////////////
void default_bad_close_handler(int fd, std::exception * ex) noexcept;

// We need to use decltype here, since typedefs & aliases don't allow
// noexcept specifiers.
using bad_close_handler_t = decltype(default_bad_close_handler);

//////////////////////////////////////////////////////////////////////
class descriptor
{
 public:
	static bad_close_handler_t * set_bad_close_handler(bad_close_handler_t *);

	descriptor(const descriptor &) = delete;
	descriptor & operator = (const descriptor &) = delete;

	descriptor(descriptor &&);
	descriptor & operator = (descriptor &&);

	virtual ~descriptor();

	virtual void close();

	virtual descriptor dup(int newFd = -1, bool close_on_exec = true);

 protected:
	descriptor(int _fd = -1) noexcept : fd(_fd) { }

	int fd;

	static bad_close_handler_t * bad_close_handler;
};

//////////////////////////////////////////////////////////////////////
template <int CLOCK>
class timer_descriptor : descriptor
{
 public:
	timer_descriptor(bool nonblock = false, bool close_on_exec = true)
	  : descriptor(timerfd_create(CLOCK, (nonblock ? TFD_NONBLOCK : 0) |
	                                      close_on_exec ? TFD_CLOEXEC : 0))
	{
		if (fd < 0) throw make_syserr("Could not create timer fd");
	}

	void arm_relative_timer(void * initial, void * interval);
	void arm_absolute_timer(void * initial, void * interval);
	void disarm_timer();

	
};

//////////////////////////////////////////////////////////////////////
class file_descriptor : public descriptor
{
 public:
	file_descriptor();
	file_descriptor(const fs_path &);

	bool open(const fs_path &);
};

class socket_descriptor { };

class signal_descriptor { };
class inotify_descriptor { };

#if 0
//////////////////////////////////////////////////////////////////////
class read_interface
{
 public:
	virtual ~read_interface() { }
	virtual int read(char * buffer, std::size_t length) = 0;
	virtual int readvec(const struct iovec * iov, int count) = 0;
};

//////////////////////////////////////////////////////////////////////
class write_interface
{
 public:
	virtual ~write_interface() { }
	virtual int write(char * buffer, std::size_t length) = 0;
	virtual int writevec(const struct iovec * iov, int count) = 0;
};

//////////////////////////////////////////////////////////////////////
class rw_interface
  : public read_interface
  , public write_interface
{
 public:
	virtual ~rw_interface() { }
};

//////////////////////////////////////////////////////////////////////
class input_descriptor : public virtual descriptor
{
 public:
	virtual ~input_descriptor() { }

	virtual int read(char * buffer, std::size_t size)
		{ return ::read(fd, buffer, size); }
};

//////////////////////////////////////////////////////////////////////
class output_descriptor : public virtual descriptor
{
 public:
	virtual ~output_descriptor() { }

	virtual int write(const char * buffer, std::size_t size)
		{ return ::write(fd, buffer, size); }
};

//////////////////////////////////////////////////////////////////////
class io_descriptor : public input_descriptor
                    , public output_descriptor
{
 public:
	virtual ~io_descriptor() { }
};
#endif

} // namespace io

#endif //GUARD_DESCRIPTOR_H
