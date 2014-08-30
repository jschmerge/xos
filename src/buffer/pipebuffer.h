#ifndef PIPEBUFFER_H
#define PIPEBUFFER_H 1

#include <fcntl.h>

#include <cstddef>
#include <atomic>

class PipeBuffer
{
 public:
	PipeBuffer(int flags = 0);

	PipeBuffer(PipeBuffer && p) noexcept;

	PipeBuffer & operator = (PipeBuffer && p) noexcept;

	~PipeBuffer();

	void swap(PipeBuffer & other) noexcept;

	int readOut(char * buffer, size_t bufsize) noexcept;

	int writeIn(const char * buffer, size_t bufsize) noexcept;

	int spliceIn(int fd, int numBytes, loff_t * offset = NULL) noexcept;

	int spliceIn(PipeBuffer & p, int numBytes) noexcept
	{
		int n = spliceIn(p.pipeFds[RD], numBytes);
		if (n > 0)
			p.fillLevel -= n;
		return n;
	}

	int spliceOut(int fd, int numBytes, loff_t * offset = NULL) noexcept;

	int spliceOut(PipeBuffer & p, int numBytes) noexcept
	{
		int n = spliceOut(p.pipeFds[WR], numBytes);
		if (n > 0)
			p.fillLevel += n;
		return n;
	}

	int spliceAllOut(int fd, loff_t * offset = NULL);

 private:
	enum { RD = 0, WR = 1 };

	void closePipe();

	int pipeFds[2];
	std::atomic<int> fillLevel;

	// We keep an fd open to /dev/null that is shared between all instances of
	// this class... Having this open allows us to dump pipe buffer content if
	// we need to. This avoids a lot of messiness when closing a pipe that
	// still has data buffered. It is declared atomic so that we avoid a race
	// condition when we create it. Probably an overkill.
	static std::atomic<int> devNullFd;

	// Disallow copying
	PipeBuffer(const PipeBuffer &) = delete;
	PipeBuffer & operator = (const PipeBuffer &) = delete;
};

#endif // PIPEBUFFER_H
