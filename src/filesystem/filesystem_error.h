#ifndef GUARD_FILESSYSTEM_ERROR_H
#define GUARD_FILESSYSTEM_ERROR_H 1

#include <memory>
#include <string>
#include <system_error>

#include "path.h"

namespace filesystem {
inline namespace v1 {

class path;

class filesystem_error : public std::system_error
{
 public:
	filesystem_error(const std::string & what_arg, std::error_code ec);

	filesystem_error(const std::string & what_arg, const path & p1,
	                 std::error_code ec);

	filesystem_error(const std::string & what_arg, const path & p1,
	                 const path & p2, std::error_code ec);

	~filesystem_error() = default;

	const path & path1() const noexcept;

	const path & path2() const noexcept;

	// Rather than override what() here, we rely on system_error::what

 protected:
	const path p1;
	const path p2;
	mutable std::shared_ptr<std::string> xxx;
};

inline std::error_code make_errno_ec()
	{ return std::error_code(errno, std::system_category()); }

inline filesystem_error make_fs_error(const std::string & msg, const path & p)
	{ return filesystem_error(msg, p, make_errno_ec()); }

inline filesystem_error make_fs_error(const std::string & msg,
                                      const path & p1, const path & p2)
	{ return filesystem_error(msg, p1, p2, make_errno_ec()); }

} // inline namespace v1
} // namespace filesystem

#endif // GUARD_FILESSYSTEM_ERROR_H
