#include "fs_operations.h"
#include "directory_iterator.h"
#include "filesystem_error.h"
#include "recursive_directory_iterator.h"
#include "../time/timeutil.h"

#include <sys/stat.h>
#include <sys/statvfs.h>
#include <fcntl.h>
#include <unistd.h>

#include <climits> // PATH_MAX is defined through this
#include <cstdlib> // for realpath()
#include <cstdio>  // P_tmpdir is defined here

namespace filesystem {
inline namespace v1 {

inline constexpr file_type st_mode_to_file_type(mode_t m)
{
	return ( (S_ISREG(m) ? file_type::regular : 
	         (S_ISDIR(m) ? file_type::directory :
	         (S_ISCHR(m) ? file_type::character :
	         (S_ISBLK(m) ? file_type::block :
	         (S_ISFIFO(m) ? file_type::fifo :
	         (S_ISLNK(m) ? file_type::symlink :
	         (S_ISSOCK(m) ? file_type::socket :
	         (file_type::unknown) ) ) ) ) ) ) ) );
}

inline constexpr perms st_mode_to_perms(mode_t m)
{
	return ( ( (m & S_IRUSR) ? perms::owner_read : perms::none )
	       | ( (m & S_IWUSR) ? perms::owner_write : perms::none )
	       | ( (m & S_IXUSR) ? perms::owner_exec : perms::none )
	       | ( (m & S_IRGRP) ? perms::group_read : perms::none )
	       | ( (m & S_IWGRP) ? perms::group_write : perms::none )
	       | ( (m & S_IXGRP) ? perms::group_exec : perms::none )
	       | ( (m & S_IROTH) ? perms::others_read : perms::none )
	       | ( (m & S_IWOTH) ? perms::others_write : perms::none )
	       | ( (m & S_IXOTH) ? perms::others_exec : perms::none )
	       | ( (m & S_ISUID) ? perms::set_uid : perms::none )
	       | ( (m & S_ISGID) ? perms::set_gid : perms::none )
	       | ( (m & S_ISVTX) ? perms::sticky_bit : perms::none ) );
}

// Note: base defaults to current_path()
//
// Also note, logic here is copied straight from thruth table in
// draft N4100 - deserves testing
// ALSO NOTE: root_name() & friends are largely untested
path absolute(const path & p, const path & base)
{
	path ret;
	if (p.has_root_name())
	{
		if (p.has_root_directory())
		{
			ret = p;
		} else
		{
			ret = p.root_name()
			      / absolute(base).root_directory()
			      / absolute(base).relative_path()
			      / p.relative_path();
		}
	} else
	{
		if (p.has_root_directory())
		{
			ret = absolute(base).root_name() / p;
		} else
		{
			ret = absolute(base) / p;
		}
	}
	return ret;
}

path canonical(const path & p, const path & base, std::error_code & ec)
{
	path ret = absolute(p, base);
	char resolved[PATH_MAX];
	char * rc = nullptr;

	// XXX - replace this with hand-rolled logic
	if ((rc = realpath(ret.c_str(), resolved)) == nullptr)
	{
		ec = make_errno_ec();
		ret.clear();
	} else
	{
		ret = resolved;
		if (!exists(ret))
		{
			ec = make_errno_ec(ENOENT);
			ret.clear();
		}
	}

	return ret;
}

path canonical(const path & p, std::error_code & ec)
{
	path base = current_path(ec);
	return ec ? path() : canonical(p, base, ec);
}

// Note: base defaults to current_path()
path canonical(const path & p, const path & base)
{
	std::error_code ec;
	path ret = canonical(p, base, ec);
	if (ec) throw filesystem_error("Could not find canonical path", p, ec);
	return ret;
}

path current_path()
{
	std::error_code ec;
	path retval = current_path(ec);
	if (ec)
		throw filesystem_error("Could not read current working directory", ec);
	return retval;
}

path current_path(std::error_code & ec)
{
	path retval;
	char cwd_buffer[PATH_MAX];
	ec.clear();

	if (getcwd(cwd_buffer, PATH_MAX) == nullptr)
		ec = make_errno_ec();
	else
		retval = cwd_buffer;

	return retval;
}

void current_path(const path & p)
{
	std::error_code ec;
	current_path(p, ec);
	if (ec)
		throw filesystem_error("Could not change working directory", p, ec);
}

void current_path(const path & p, std::error_code & ec) noexcept
{
	ec.clear();

	if (chdir(p.c_str()) != 0)
		ec = make_errno_ec();
}

void copy_symlink(const path & from, const path & to)
{
	std::error_code ec;
	copy_symlink(from, to, ec);
	if (ec) throw filesystem_error("Could not copy symlink", from, to, ec);
}

void copy_symlink(const path & from, const path & to,
                  std::error_code & ec) noexcept
{
	path p = read_symlink(from, ec);

	if (!ec)
		create_symlink(p, to, ec);
}

bool create_directories(const path & p)
{
	std::error_code ec;
	bool rc = create_directories(p, ec);
	if (ec)
		throw filesystem_error("Could not create directory and ancestors",
	                           p, ec);
	return rc;
}

bool create_directories(const path & p, std::error_code & ec) noexcept
{
	path tmp_path;
	size_t creation_count = 0;

	for (const auto & elem : p)
	{
		tmp_path /= elem;

		if (create_directory(tmp_path, ec))
			++creation_count;

		if (ec)
			break;
	}

	return (creation_count > 0);
}

bool create_directory(const path & p)
{
	std::error_code ec;
	bool rc = create_directory(p, ec);
	if (ec) throw filesystem_error("Could not create directory", p, ec);
	return rc;
}

bool create_directory(const path & p, std::error_code & ec,
                      perms dir_perms) noexcept
{
	bool rc = false;
	if (mkdir(p.c_str(), static_cast<mode_t>(dir_perms)) == 0)
	{
		rc = true;
	} else if (errno == EEXIST) // failure due to existence is not error
	{
		if (is_directory(p, ec))
			ec.clear();
		else
			ec = make_errno_ec(EEXIST);
	} else
	{
		ec = make_errno_ec();
	}

	return rc;
}

bool create_directory(const path & p, std::error_code & ec) noexcept
{
	return create_directory(p, ec, perms::all);
}

bool create_directory(const path & p, const path & template_path)
{
	std::error_code ec;
	bool rc = create_directory(p, template_path, ec);
	if (ec) throw filesystem_error("Could not create directory", p, ec);
	return rc;
}

bool create_directory(const path & p, const path & template_path,
                      std::error_code & ec) noexcept
{
	file_status st = status(template_path, ec);
	if (ec) return false;
	return create_directory(p, ec, st.permissions() & perms::mask);
}

void create_directory_symlink(const path & oldpath, const path & newpath)
{
	create_symlink(oldpath, newpath);
}

void create_directory_symlink(const path & oldpath, const path & newpath,
                              std::error_code & ec) noexcept
{
	create_symlink(oldpath, newpath, ec);
}

void create_hard_link(const path & oldpath, const path & newpath)
{
	std::error_code ec;
	create_hard_link(oldpath, newpath, ec);
	if (ec)
		throw filesystem_error("Could not create hard link",
		                       oldpath, newpath, ec);
}

void create_hard_link(const path & oldpath, const path & newpath,
                      std::error_code & ec) noexcept
{
	if (link(oldpath.c_str(), newpath.c_str()))
		ec = make_errno_ec();
}

void create_symlink(const path & oldpath, const path & newpath)
{
	std::error_code ec;
	create_symlink(oldpath, newpath, ec);
	if (ec)
		throw filesystem_error("Could not create symlink",
		                       oldpath, newpath, ec);
}

void create_symlink(const path & oldpath, const path & newpath,
                    std::error_code & ec) noexcept
{
	if (symlink(oldpath.c_str(), newpath.c_str()) != 0)
		ec = make_errno_ec();
}

bool equivalent(const path & p1, const path & p2)
{
	std::error_code ec;
	bool rc = equivalent(p1, p2, ec);
	if (ec) throw filesystem_error("Could not determine equivalence",
	                               p1, p2, ec);
	return rc;
}

bool equivalent(const path & p1, const path & p2,
                std::error_code & ec) noexcept
{
	bool rc = false;
	struct stat st1, st2;
	memset(&st1, 0, sizeof(st1));
	memset(&st2, 0, sizeof(st2));
	if ((stat(p1.c_str(), &st1) == 0) && (stat(p2.c_str(), &st2) == 0))
	{
		if ((st1.st_dev == st2.st_dev) && (st1.st_ino == st2.st_ino))
			rc = true;
	} else
		ec = make_errno_ec();

	return rc;
}

bool exists(const path & p)
{
	return exists(status(p));
}

bool exists(const path & p, std::error_code & ec) noexcept
{
	return exists(status(p, ec));
}

uintmax_t file_size(const path & p)
{
	std::error_code  ec;
	uintmax_t ret = file_size(p, ec);
	if (ec) throw filesystem_error("Could not read file size", p, ec);
	return ret;
}

uintmax_t file_size(const path & p, std::error_code & ec) noexcept
{
	struct stat st;
	uintmax_t ret = static_cast<uintmax_t>(-1);

	ec.clear();

	if (stat(p.c_str(), &st) == 0)
	{
		if (S_ISREG(st.st_mode))
			ret = st.st_size;
		else
			ec = std::make_error_code(std::errc::not_supported);
	} else
		ec = make_errno_ec();

	return ret;
}

uintmax_t hard_link_count(const path & p)
{
	std::error_code  ec;
	uintmax_t ret = hard_link_count(p, ec);
	if (ec) throw filesystem_error("Could not read link count", p, ec);
	return ret;
}

uintmax_t hard_link_count(const path & p, std::error_code & ec) noexcept
{
	struct stat st;
	uintmax_t ret = static_cast<uintmax_t>(-1);

	ec.clear();

	if (stat(p.c_str(), &st) == 0)
		ret = st.st_nlink;
	else
		ec = make_errno_ec();

	return ret;
}

bool is_block_file(const path & p)
	{ return is_block_file(status(p)); }

bool is_block_file(const path & p, std::error_code & ec) noexcept
	{ return is_block_file(status(p, ec)); }

bool is_character_file(const path & p)
	{ return is_character_file(status(p)); }

bool is_character_file(const path & p, std::error_code & ec) noexcept
	{ return is_character_file(status(p, ec)); }

bool is_directory(const path & p)
	{ return is_directory(status(p)); }

bool is_directory(const path & p, std::error_code & ec) noexcept
	{ return is_directory(status(p, ec)); }

bool is_empty(const path & p)
{
	std::error_code ec;
	bool rc = is_empty(p, ec);
	if (ec) throw filesystem_error("Could not get size of directory", p, ec);
	return rc;
}

bool is_empty(const path & p, std::error_code & ec) noexcept
{
	bool rc = false;
	if (is_directory(p, ec))
	{
		rc = (directory_iterator(p) == directory_iterator());
	} else
	{
		rc = (file_size(p, ec) == 0);
	}

	if (ec) rc = false;
	return rc;
}

bool is_fifo(const path & p)
	{ return is_fifo(status(p)); }

bool is_fifo(const path & p, std::error_code & ec) noexcept
	{ return is_fifo(status(p, ec)); }

bool is_other(const path & p)
	{ return is_other(symlink_status(p)); }

bool is_other(const path & p, std::error_code & ec) noexcept
	{ return is_other(symlink_status(p, ec)); }

bool is_regular_file(const path & p)
	{ return is_regular_file(status(p)); }

bool is_regular_file(const path & p, std::error_code & ec) noexcept
	{ return is_regular_file(status(p, ec)); }

bool is_socket(const path & p)
	{ return is_socket(status(p)); }

bool is_socket(const path & p, std::error_code & ec) noexcept
	{ return is_socket(status(p, ec)); }

bool is_symlink(const path & p)
	{ return is_symlink(symlink_status(p)); }

bool is_symlink(const path & p, std::error_code & ec) noexcept
	{ return is_symlink(symlink_status(p, ec)); }

file_time_type last_write_time(const path & p)
{
	std::error_code ec;
	file_time_type ret = last_write_time(p, ec);
	if (ec)
		throw filesystem_error("Could not read file modification time",
		                       p, ec);
	return ret;
}

file_time_type last_write_time(const path & p, std::error_code & ec) noexcept
{
	struct stat st;
	file_time_type ret{file_time_type::min()};

	ec.clear();

	if (stat(p.c_str(), &st) == 0)
		ret = from_timespec<file_time_type::clock,
		                    file_time_type::duration>(st.st_mtim);
	else
		ec = make_errno_ec();

	return ret;
}


void last_write_time(const path & p, file_time_type new_time)
{
	std::error_code ec;
	last_write_time(p, new_time, ec);
	if (ec) throw filesystem_error("Could not set modification time", p, ec);
}

void last_write_time(const path & p, file_time_type new_time,
                      std::error_code & ec) noexcept
{
	// XXX - flags to system call here allow distinguishing symlink from
	// underlying object - should we expose this?
	struct timespec times[2] = { { 0, UTIME_OMIT }, { 0, 0 } };
	times[1] = to_timespec(new_time);

	if (utimensat(AT_FDCWD, p.c_str(), times, 0) != 0)
		ec = make_errno_ec();
}

void permissions(const path & p, perms prms)
{
	std::error_code ec;
	permissions(p, prms);
	if (ec) throw filesystem_error("Could not set permissions", p, ec);
}

void permissions(const path & p, perms prms, std::error_code & ec) noexcept
{
	perms newperms = status(p, ec).permissions();

	if (ec) return; // bail out if we couldn't stat

	if ((prms & perms::add_remove_mask) == perms::add_perms)
	{
		newperms |= (prms & perms::mask);

	} else if ((prms & perms::add_remove_mask) == perms::remove_perms)
	{
		newperms &= ~(prms & perms::mask);

	} else if ((prms & perms::add_remove_mask) == perms::none)
	{
		newperms = (prms & perms::mask);
	} else
	{
		// both add and remove are set
		ec = make_errno_ec(EINVAL);
		return;
	}

	// XXX - flags to system call here allow distinguishing symlink from
	// underlying object - should we expose this?
	if (fchmodat(AT_FDCWD, p.c_str(), static_cast<mode_t>(newperms), 0) != 0)
	{
		ec = make_errno_ec();
	}
}

path read_symlink(const path & p)
{
	std::error_code ec;
	path retpath = read_symlink(p, ec);
	if (ec) throw filesystem_error("Could not read symlink", p, ec);
	return retpath;
}

path read_symlink(const path & p, std::error_code & ec)
{
	char buffer[PATH_MAX];
	ssize_t sz;
	path ret;

	sz = readlink(p.c_str(), buffer, PATH_MAX);

	if (sz <= 0)
		ec = make_errno_ec();
	else
		ret = buffer;

	return ret;
}

bool remove(const path & p)
{
	std::error_code ec;
	bool rc = remove(p, ec);
	if (!rc || ec) throw filesystem_error("Could not remove path", p, ec);
	return rc;
}

bool remove(const path & p, std::error_code & ec) noexcept
{
	int rc = 0;
	file_status st;
	ec.clear();
	st = symlink_status(p, ec);
	if (ec) return false;
	rc = is_directory(st) ? rmdir(p.c_str()) : unlink(p.c_str());
	if (rc != 0) ec = make_errno_ec();
	return (rc == 0);
}

uintmax_t remove_all(const path & p)
{
	std::error_code ec;
	uintmax_t count = remove_all(p, ec);
	if (ec) throw filesystem_error("remove_all failed", p, ec);
	return count;
}

uintmax_t remove_all(const path & p, std::error_code & ec) noexcept
{
	uintmax_t count = 0;
	path last_directory;
	int last_depth = 0;
	std::vector<path> stack; // for remembering on recursion

	ec.clear();

	for (recursive_directory_iterator rdi(p, ec);
	     (!ec) && rdi != recursive_directory_iterator();
	     rdi.increment(ec))
	{
//		printf("\n--------> %s (last=%d, curr=%d)", rdi->path().c_str(),
//		       last_depth, rdi.depth());

		if (last_depth > rdi.depth())
		{
			// just finished recursing
//			printf("\nRemoving directory %s\n", stack.back().c_str());
			remove(stack.back(), ec);
			stack.pop_back();
		}

		file_status st = rdi->symlink_status(ec);

		if (! ec)
		{
			if ( st.type() == file_type::directory )
			{
				if (!is_linking_directory(*rdi))
					stack.push_back(*rdi);
			} else
			{
//				printf("\nRemoving entry %s\n", rdi->path().c_str());
				remove(rdi->path(),ec);
			}
		}

		last_depth = rdi.depth();
	}

	if ( ! ec )
	{
//		printf("\nRemoving target %s\n", p.c_str());
		remove(p, ec);
	}

	return count;
}

void rename(const path & from, const path & to)
{
	std::error_code ec;
	rename(from, to, ec);
	if (ec) throw filesystem_error("Could not rename file", from, to, ec);
}

void rename(const path & from, const path & to, std::error_code & ec) noexcept
{
	if (renameat(AT_FDCWD, from.c_str(), AT_FDCWD, to.c_str()) != 0)
		ec = make_errno_ec();
}

void resize_file(const path & p, uintmax_t size)
{
	std::error_code ec;
	resize_file(p, size, ec);
	if (ec) throw filesystem_error("Could not trucate/resize file", p, ec);
}

void resize_file(const path & p, uintmax_t size,
                 std::error_code & ec) noexcept
{
	if (truncate(p.c_str(), size) != 0)
		ec = make_errno_ec();
}

space_info space(const path & p)
{
	std::error_code ec;
	space_info info = space(p, ec);
	if (ec) throw filesystem_error("Could not determine size information "
	                               "for filesystem", p, ec);
	return info;
}

space_info space(const path & p, std::error_code & ec) noexcept
{
	struct statvfs stvfs;
	space_info info = { static_cast<uintmax_t>(-1),
	                    static_cast<uintmax_t>(-1),
	                    static_cast<uintmax_t>(-1) };

	memset(&stvfs, 0, sizeof(stvfs));

	ec.clear();

	if (statvfs(p.c_str(), &stvfs) == 0)
	{
		info.capacity = stvfs.f_blocks;
		info.free = stvfs.f_bfree;
		info.available = stvfs.f_bavail;

		info.capacity *= stvfs.f_frsize;
		info.free *= stvfs.f_frsize;
		info.available *= stvfs.f_frsize;
	} else
		ec = make_errno_ec();

	return info;
}

file_status status(const path & p, std::error_code & ec) noexcept
{
	struct stat st;
	file_status ret;

	ec.clear();

	if (stat(p.c_str(), &st) == 0)
	{
		ret.type(st_mode_to_file_type(st.st_mode));
		ret.permissions(st_mode_to_perms(st.st_mode));
	} else if (errno == ENOENT)
		ret.type(file_type::not_found);
	else
		ec = make_errno_ec();

	return ret;
}

file_status status(const path & p)
{
	std::error_code  ec;
	file_status ret = status(p, ec);
	if (ec) throw filesystem_error("Could not stat file", p, ec);
	return ret;
}

file_status symlink_status(const path & p)
{
	std::error_code  ec;
	file_status ret = symlink_status(p, ec);
	if (ec) throw filesystem_error("Could not lstat file", p, ec);
	return ret;
}

file_status symlink_status(const path & p, std::error_code & ec) noexcept
{
	struct stat st;
	file_status ret;

	ec.clear();

	if (lstat(p.c_str(), &st) == 0)
	{
		ret.type(st_mode_to_file_type(st.st_mode));
		ret.permissions(st_mode_to_perms(st.st_mode));
	} else if (errno == ENOENT)
		ret.type(file_type::not_found);
	else
		ec = make_errno_ec();

	return ret;
}

path system_complete(const path & p)
{
	return absolute(p, current_path());
}

path system_complete(const path & p, std::error_code & ec)
{
	ec.clear();
	return absolute(p, current_path());
}

path temp_directory_path()
{
	std::error_code  ec;
	path ret = temp_directory_path(ec);
	if (ec) throw filesystem_error("No suitable temp directory found", ec);
	return ret;
}

path temp_directory_path(std::error_code & ec)
{
	path tmpdir{P_tmpdir};
	file_status st;

	for (const auto var : { "TMPDIR", "TMP", "TEMP", "TEMPDIR" })
	{
		const char * value = getenv(var);
		if (value != nullptr)
		{
			printf("Found var %s = %s\n", var, value);
			st = status(value, ec);

			if (exists(st) && is_directory(st))
			{
				tmpdir = value;
				break;
			}
		}
	}

	ec.clear();
	st = status(tmpdir, ec);
	if (!exists(st) || !is_directory(st))
		tmpdir.clear();

	return tmpdir;
}

}
}
