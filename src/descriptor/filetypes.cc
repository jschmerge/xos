#include <sys/types.h>
#include <sys/signalfd.h>
#include <sys/timerfd.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>

void error(const char * fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fprintf(stderr, ": %s\n", strerror(errno));
	exit(1);
}

void fdInfo(int fd, const char * desc)
{
	struct stat st;

	if (fstat(fd, &st) != 0)
		error("Could not stat %s", desc);

	printf("-> %s: %012o\n", desc, st.st_mode);
	printf("\tdev  = %d:%d\n", major(st.st_dev), minor(st.st_dev));
	printf("\trdev = %d:%d\n", major(st.st_rdev), minor(st.st_rdev));
	printf("\tS_ISREG  is %s\n", S_ISREG(st.st_mode) ? "true" : "false");
	printf("\tS_ISDIR  is %s\n", S_ISDIR(st.st_mode) ? "true" : "false");
	printf("\tS_ISCHR  is %s\n", S_ISCHR(st.st_mode) ? "true" : "false");
	printf("\tS_ISBLK  is %s\n", S_ISBLK(st.st_mode) ? "true" : "false");
	printf("\tS_ISFIFO is %s\n", S_ISFIFO(st.st_mode) ? "true" : "false");
	printf("\tS_ISSOCK is %s\n", S_ISSOCK(st.st_mode) ? "true" : "false");
}

int main(int , char ** argv)
{
	int fd;

	fdInfo(0, "stdin");
	fdInfo(1, "stdout");
	fdInfo(2, "stderr");

	if ((fd = open(argv[0], O_RDONLY)) < 0) error(argv[0]);
	fdInfo(fd, "regular file");
	close(fd);

	if ((fd = open(argv[0], O_RDONLY)) < 0) error(argv[0]);
	fdInfo(fd, "regular file");
	close(fd);

	if ((fd = open(".", O_RDONLY)) < 0) error(".");
	fdInfo(fd, "directory");
	close(fd);

	if ((fd = epoll_create1(0)) < 0) error("epoll_create");
	fdInfo(fd, "epoll");
	close(fd);

	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	if ((fd = signalfd(-1, &mask, 0)) < 0) error("signalfd");
	fdInfo(fd, "signalfd");
	close(fd);

	if ((fd = timerfd_create(CLOCK_REALTIME, 0)) < 0) error("timerfd_create");
	fdInfo(fd, "timerfd");
	close(fd);

	return 0;
}
