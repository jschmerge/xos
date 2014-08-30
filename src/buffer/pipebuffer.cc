#include "pipebuffer.h"
#include "util.h"

#include <unistd.h>
#include <fcntl.h>

#include <cstdio>
#include <cstring>
#include <cerrno>

//////////////////////////////////////////////////////////////////////
std::atomic<int> PipeBuffer::devNullFd(-1);

//////////////////////////////////////////////////////////////////////
PipeBuffer::PipeBuffer(int flags)
  : pipeFds{-1, -1}
  , fillLevel(0)
{
	// Open the static devNullFd if it hasn't been opened
	int nullFd;
	int tmp = -1;

	if ((nullFd = devNullFd.load()) < 0)
	{
		if ((nullFd = open("/dev/null", O_RDWR)) < 0)
			throw make_syserr(errno, "open of /dev/null failed");

		if (!devNullFd.compare_exchange_strong(tmp, nullFd))
			close(nullFd);
	}
		
	if (pipe2(pipeFds, flags) < 0)
		throw make_syserr(errno, "Pipe creation failed");
}

//////////////////////////////////////////////////////////////////////
PipeBuffer::PipeBuffer(PipeBuffer && p) noexcept
{
	this->swap(p);
}

//////////////////////////////////////////////////////////////////////
PipeBuffer & PipeBuffer::operator = (PipeBuffer && p) noexcept
{
	if (this != &p) this->swap(p);
	return *this;
}

//////////////////////////////////////////////////////////////////////
PipeBuffer::~PipeBuffer()
{
//	fprintf(stderr, "fillLevel = %d\n", fillLevel);

	closePipe(); // XXX - this can throw
}

//////////////////////////////////////////////////////////////////////
void PipeBuffer::swap(PipeBuffer & other) noexcept
{
	using std::swap;

	swap(pipeFds[RD], other.pipeFds[RD]);
	swap(pipeFds[WR], other.pipeFds[WR]);
	int tmp = fillLevel;
	fillLevel.store(other.fillLevel);
	other.fillLevel.store(tmp);
}

//////////////////////////////////////////////////////////////////////
int PipeBuffer::readOut(char * buffer, size_t bufsize) noexcept
{
	int n = read(pipeFds[RD], buffer, bufsize);
	if (n > 0)
		fillLevel -= n;
	return n;
}

//////////////////////////////////////////////////////////////////////
int PipeBuffer::writeIn(const char * buffer, size_t bufsize) noexcept
{
	int n = write(pipeFds[WR], buffer, bufsize);
	if (n > 0)
		fillLevel += n;
	return n;
}

//////////////////////////////////////////////////////////////////////
int PipeBuffer::spliceIn(int fd, int numBytes, loff_t * offset) noexcept
{
	int n = splice(fd, offset, pipeFds[WR], NULL,
	               numBytes, SPLICE_F_MOVE);
	if (n > 0)
		fillLevel += n;
	return n;
}

//////////////////////////////////////////////////////////////////////
int PipeBuffer::spliceOut(int fd, int numBytes, loff_t * offset) noexcept
{
	int n = splice(pipeFds[RD], NULL, fd, offset,
	               numBytes, SPLICE_F_MOVE);
	if (n > 0)
		fillLevel -= n;
	return n;
}

//////////////////////////////////////////////////////////////////////
int PipeBuffer::spliceAllOut(int fd, loff_t * offset)
{
	int n = 0;
	loff_t localOffset = 0;
	loff_t * localOffsetPtr = NULL;

	if (offset != NULL)
	{
		localOffset = *offset;
		localOffsetPtr = &localOffset;
	}

	while (fillLevel > 0)
	{
		int rc = spliceOut(fd, fillLevel, localOffsetPtr);

		if (rc > 0)
		{
			n += rc;

			if (localOffsetPtr != NULL)
				localOffset += rc;
		} else
			break;
	}

	return n;
}

//////////////////////////////////////////////////////////////////////
void PipeBuffer::closePipe()
{

	// Sanity note: from the manpage: If all file descriptors referring
	// to the read end of a pipe have been closed, then a write(2) will
	// cause  a  SIGPIPE signal to be generated for the calling process.
	//
	// We close the write end before the read end for this reason.
	if (close(pipeFds[WR]) < 0)
		throw make_syserr(errno, "close of write end of pipe failed");

	spliceAllOut(devNullFd, NULL);

	if (close(pipeFds[RD]) < 0)
		throw make_syserr(errno, "close of read end of pipe failed");
}
