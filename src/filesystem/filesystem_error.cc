#include "path.h"
#include "filesystem_error.h"

namespace filesystem {
inline namespace v1 {

filesystem_error::filesystem_error(const std::string & what_arg,
                                   std::error_code ec)
  : std::system_error(ec, what_arg), p1(), p2()
	{ }

filesystem_error::filesystem_error(const std::string & what_arg,
                                   const path & p,
                                   std::error_code ec)
  : std::system_error(ec, what_arg + " (target = " + p.c_str() + ")")
  , p1(p), p2()
	{ }

filesystem_error::filesystem_error(const std::string & what_arg,
                                   const path & _p1, const path & _p2,
                                   std::error_code ec)
  : std::system_error(ec, what_arg +
                      " (source = " + _p1.c_str() +
                      ", dest = "+ _p2.c_str() + ")")
  , p1(_p1), p2(_p2)
	{ }

const path & filesystem_error::path1() const noexcept
	{ return p1; }

const path & filesystem_error::path2() const noexcept
	{ return p2; }

} // inline namespace v1
} // namespace filesystem
