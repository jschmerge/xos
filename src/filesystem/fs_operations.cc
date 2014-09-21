#include "fs_operations.h"
#include "filesystem_error.h"

#include <sys/stat.h>
#include <unistd.h>


namespace filesystem {
inline namespace v1 {

constexpr file_type st_mode_to_file_type(mode_t m)
{
	return (S_ISREG(m) ? file_type::regular : 
	        S_ISDIR(m) ? file_type::directory :
	        S_ISCHR(m) ? file_type::character :
	        S_ISBLK(m) ? file_type::block :
	        S_ISFIFO(m) ? file_type::fifo :
	        S_ISLNK(m) ? file_type::symlink :
	        S_ISSOCK(m) ? file_type::socket :
	        file_type::unknown);
}

file_status status(const path & p)
{
	struct stat st;

	if (stat(p.c_str(), &st) != 0)
		throw make_fs_error("Could not stat file", p);

	file_status ret(st_mode_to_file_type(st.st_mode));

	return ret;
}

file_status status(const path & p, std::error_code & ec) noexcept
{
	file_status ret;

	return ret;
}

}
}
