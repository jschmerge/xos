#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <cstdio>
#include <cstring>

#include <string>

#include "util.h"

// const off_t chunkSize = (1024 * 1024 * 128);
//const off_t overlapSize = (1024 * 1024 * 64);

struct MmapWindow
{
	static constexpr int defaultFlags =
		(MAP_PRIVATE | MAP_POPULATE | MAP_NORESERVE);

	MmapWindow(int _fd, off_t _start,
	           size_t _length, int _flags = defaultFlags)
	  : fd(_fd)
	  , mapping(nullptr)
	  , flags(_flags)
	  , offset(_start)
	  , length(_length)
	{
		if (MAP_FAILED != (mapping = mmap(nullptr, length, PROT_READ,
		                                  flags, fd, offset)))
		{
			mapping = nullptr;
			throw make_syserr("mmap failed");
		}
	}

	~MmapWindow()
	{
		int rc = 0;
		if (mapping != nullptr)
		{
			rc = munmap(mapping, length);
		}

		if (rc != 0)
		{
		}
	}

	int fd;
	void * mapping;
	int flags;
	off_t offset;
	size_t length;
};

class MmapFileReader
{
 public:
	MmapFileReader(const char * fname)
	  : fd(-1)
	{
		if ((fd = open(fname, O_RDONLY)) < 0)
			throw make_syserr(S("Could not open ") + fname);

		if (fstat(fd, &statInfo) != 0)
			throw make_syserr("stat failed");
	}

	~MmapFileReader()
	{
		myClose();
		fd = -1;
	}

	void myClose()
	{
		if (fd >= 0)
		{
			if (::close(fd) != 0)
				throw make_syserr("close failed");
		}
	}

 protected:
	int fd;
	struct stat statInfo;
};

int main(int argc, char ** argv)
{
	for (int i = 1; i < argc; ++i)
	{
		printf("reading file %s\n", argv[i]);
		MmapFileReader infile(argv[i]);
	}

	return 0;
}
