#include "fs_operations.h"
#include "filesystem_error.h"

#include <sys/stat.h>
#include <unistd.h>

#include <climits> // PATH_MAX is defined through this


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

void current_path(const path& p);
void current_path(const path& p, std::error_code& ec) noexcept;

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

path temp_directory_path();
path temp_directory_path(std::error_code& ec);

}
}
