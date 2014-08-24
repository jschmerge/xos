/*
bdflush(2)
flock(2)
futex(2)
getpid(2)
gettid(2)
link(2)
lockf(3)
lseek(2)
mknod(2)
mmap(2)
msgctl(2)
semctl(2)
shmctl(2)
mount(2)
openat(2)
pipe(2)
ptrace(2)
read(2)
set_thread_area(2)
set_tid_address(2)
shutdown(2)
socket(2)
sync(2)
sync_file_range(2)
stat(2)
tkill(2)
umask(2)
unshare(2)
unlink(2)
wait(2)
write(2)

(read, pread, readv)
(write, pwrite, writev, trun cate, ftruncate).
(open, creat, mknod, mkdir, link, symlink, rename).
(unlink,  rename  to another directory, rmdir).
(rename).
(chown, chmod, utime[s]).

capabilities(7)
feature_test_macros(7)
fifo(7)
path_resolution(7)
pthreads(7)
symlink(7)

mount(8)
sync(8)
update(8)
hdparm(8)
ld.so(8)
*/

//////////////////////////////////////////////////////////////////////
// chmod(2)
#include <sys/stat.h>

int chmod(const char *path, mode_t mode);
int fchmod(int fd, mode_t mode);

//////////////////////////////////////////////////////////////////////
// chown(2)
#include <unistd.h>

int chown(const char *path, uid_t owner, gid_t group);
int fchown(int fd, uid_t owner, gid_t group);
int lchown(const char *path, uid_t owner, gid_t group);

//////////////////////////////////////////////////////////////////////
// close(2)
#include <unistd.h>

int close(int fd);

//////////////////////////////////////////////////////////////////////
// dup(2)
#include <unistd.h>

int dup(int oldfd);
int dup2(int oldfd, int newfd);

#ifndef _GNU_SOURCE
#define _GNU_SOURCE             /* See feature_test_macros(7) */
#endif
#include <fcntl.h>              /* Obtain O_* constant definitions */
#include <unistd.h>

int dup3(int oldfd, int newfd, int flags);

//////////////////////////////////////////////////////////////////////
// execve(2)
#include <unistd.h>

int execve(const char *filename, char *const argv[], char *const envp[]);

//////////////////////////////////////////////////////////////////////
// execl(3)
#include <unistd.h>

extern char **environ;

int execl(const char *path, const char *arg, ...);
int execlp(const char *file, const char *arg, ...);
int execle(const char *path, const char *arg, .../* , char * const envp[]*/);
int execv(const char *path, char *const argv[]);
int execvp(const char *file, char *const argv[]);
int execvpe(const char *file, char *const argv[], char *const envp[]);

//////////////////////////////////////////////////////////////////////
// fchmodat(2)
#include<fcntl.h>           /* Definition of AT_* constants */
#include <sys/stat.h>

int fchmodat(int dirfd, const char *pathname, mode_t mode, int flags);

//////////////////////////////////////////////////////////////////////
// fchownat(2)
#include <fcntl.h> /* Definition of AT_* constants */
#include <unistd.h>

int fchownat(int dirfd, const char *pathname,
             uid_t owner, gid_t group, int flags);

//////////////////////////////////////////////////////////////////////
// fexecve(3)
#include <unistd.h>

int fexecve(int fd, char *const argv[], char *const envp[]);

//////////////////////////////////////////////////////////////////////
// fcntl(2)

#include <unistd.h>
#include <fcntl.h>

int fcntl(int fd, int cmd, ... /* arg */ );

/* OPTIONS
 Duplicating a file descriptor
	F_DUPFD (int)
	F_DUPFD_CLOEXEC (int; since Linux 2.6.24)

 File descriptor flags
	F_GETFD (void)
	F_SETFD (int)

 File status flags
	F_GETFL (void)
	F_SETFL (int)

 Advisory locking
	struct flock {
		short l_type;    // F_RDLCK, F_WRLCK, or F_UNLCK
		short l_whence;  // SEEK_SET, SEEK_CUR, SEEK_END 
		off_t l_start;   // Starting offset for lock
		off_t l_len;     // Number of bytes to lock
		pid_t l_pid;     // PID of process blocking our lock (F_GETLK only)
	};
	F_SETLK (struct flock *)
	F_SETLKW (struct flock *)
	F_GETLK (struct flock *)

 Managing signals
	struct f_owner_ex {
		int   type;
		pid_t pid;
	};
	F_GETOWN (void)
	F_SETOWN (int)
	F_GETOWN_EX (struct f_owner_ex *) (since Linux 2.6.32)
	F_SETOWN_EX (struct f_owner_ex *) (since Linux 2.6.32)
	F_GETSIG (void)
	F_SETSIG (int)

 Leases
	F_SETLEASE (int)
	F_GETLEASE (void)

 File and directory change notification (dnotify)
	F_NOTIFY (int)

 Changing the capacity of a pipe
	F_SETPIPE_SZ (int; since Linux 2.6.35)
	F_GETPIPE_SZ (void; since Linux 2.6.35)
*/

//////////////////////////////////////////////////////////////////////
// fsync(2)
#include <unistd.h>

int fsync(int fd);
int fdatasync(int fd);

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// open(2)
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int open(const char *pathname, int flags);
int open(const char *pathname, int flags, mode_t mode);
int creat(const char *pathname, mode_t mode);

