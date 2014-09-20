#include "fs_funcs.h"


namespace filesystem {
inline namespace v1 {

bool exists(file_status s) noexcept;
bool exists(const path& p);
bool exists(const path& p, std::error_code& ec) noexcept;

}
}
