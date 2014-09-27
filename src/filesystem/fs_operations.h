#ifndef GUARD_FS_FUNCS_H
#define GUARD_FS_FUNCS_H 1

#include <chrono>
#include <system_error>

#include "../utility/bitmask_operators.h"
#include "path.h"
#include "file_status.h"

namespace filesystem {
inline namespace v1 {

class file_status;

struct space_info
{
	uintmax_t capacity;
	uintmax_t free;
	uintmax_t available;
};

enum class copy_options
{
	none = 0,

	skip_existing      = (1 << 0),
	overwrite_existing = (1 << 1),
	update_existing    = (1 << 2),

	recursive          = (1 << 3),

	copy_symlinks      = (1 << 4),
	skip_symlinks      = (1 << 5),

	directories_only   = (1 << 6),
	create_symlinks    = (1 << 7),
	create_hard_links  = (1 << 8),
};

DEFINE_BITMASK_OPERATORS(copy_options, unsigned int);

typedef std::chrono::high_resolution_clock clock_type;
typedef std::chrono::time_point<clock_type> file_time_type;

path current_path();
path current_path(std::error_code & ec);
void current_path(const path & p);
void current_path(const path & p, std::error_code & ec) noexcept;

path absolute(const path & p, const path & base = current_path());

path canonical(const path & p, const path & base, std::error_code & ec);
path canonical(const path & p, std::error_code & ec);
path canonical(const path & p, const path & base = current_path());

#if 0
void copy(const path & from, const path & to)
{
	copy(from, to, copy_options::none);
}

void copy(const path & from, const path & to, std::error_code & ex) noexcept
{
	copy(from, to, copy_options::none, ec);
}

void copy(const path & from, const path & to, copy_options options);
void copy(const path & from, const path & to, copy_options options,
          std::error_code & ex) noexcept;

bool copy_file(const path & from, const path & to);
{
	return copy_file(from, to, copy_options::none);
}

bool copy_file(const path & from, const path & to,
               std::error_code & ec) noexcept;
{
	return copy_file(from, to, copy_options::none, ec);
}

bool copy_file(const path & from, const path & to, copy_options options);
bool copy_file(const path & from, const path & to, copy_options options,
               std::error_code & ec) noexcept;
#endif

void copy_symlink(const path & existing, const path & newborn);
void copy_symlink(const path & existing, const path & newborn,
                  std::error_code & ec) noexcept;

bool create_directories(const path & p);
bool create_directories(const path & p, std::error_code & ec) noexcept;

bool create_directory(const path & p);
bool create_directory(const path & p, std::error_code & ec) noexcept;

bool create_directory(const path & p, const path & attributes);
bool create_directory(const path & p, const path & attributes,
                      std::error_code & ec) noexcept;

void create_directory_symlink(const path & to, const path & new_symlink);
void create_directory_symlink(const path & to, const path & new_symlink,
                              std::error_code & ec) noexcept;

void create_hard_link(const path & to, const path & new_hard_link);
void create_hard_link(const path & to, const path & new_hard_link,
                      std::error_code & ec) noexcept;

void create_symlink(const path & to, const path & new_symlink);
void create_symlink(const path & to, const path & new_symlink,
                    std::error_code & ec) noexcept;

bool exists(const path & p);
bool exists(const path & p, std::error_code & ec) noexcept;

bool equivalent(const path & p1, const path & p2);
bool equivalent(const path & p1, const path & p2,
                std::error_code & ec) noexcept;

uintmax_t file_size(const path & p);
uintmax_t file_size(const path & p, std::error_code & ec) noexcept;

uintmax_t hard_link_count(const path & p);
uintmax_t hard_link_count(const path & p, std::error_code & ec) noexcept;

bool is_block_file(const path & p);
bool is_block_file(const path & p, std::error_code & ec) noexcept;

bool is_character_file(const path & p);
bool is_character_file(const path & p, std::error_code & ec) noexcept;

bool is_directory(const path & p);
bool is_directory(const path & p, std::error_code & ec) noexcept;

bool is_empty(const path & p);
bool is_empty(const path & p, std::error_code & ec) noexcept;

bool is_fifo(const path & p);
bool is_fifo(const path & p, std::error_code & ec) noexcept;

bool is_other(const path & p);
bool is_other(const path & p, std::error_code & ec) noexcept;

bool is_regular_file(const path & p);
bool is_regular_file(const path & p, std::error_code & ec) noexcept;

bool is_socket(const path & p);
bool is_socket(const path & p, std::error_code & ec) noexcept;

bool is_symlink(const path & p);
bool is_symlink(const path & p, std::error_code & ec) noexcept;

file_time_type last_write_time(const path & p);
file_time_type last_write_time(const path & p, std::error_code & ec) noexcept;

#if 0
void last_write_time(const path & p, file_time_type new_time);
void last_write_time(const path & p, file_time_type new_time,
                      std::error_code & ec) noexcept;

void permissions(const path & p, perms prms);
void permissions(const path & p, perms prms, std::error_code & ec) noexcept;
#endif

path read_symlink(const path & p);
path read_symlink(const path & p, std::error_code & ec);

bool remove(const path & p);
bool remove(const path & p, std::error_code & ec) noexcept;

#if 0
uintmax_t remove_all(const path & p);
uintmax_t remove_all(const path & p, std::error_code & ec) noexcept;

void rename(const path & from, const path & to);
void rename(const path & from, const path & to, std::error_code & ec) noexcept;
#endif

void resize_file(const path & p, uintmax_t size);
void resize_file(const path & p, uintmax_t size, std::error_code & ec) noexcept;

space_info space(const path & p);
space_info space(const path & p, std::error_code & ec) noexcept;

file_status status(const path & p);
file_status status(const path & p, std::error_code & ec) noexcept;

file_status symlink_status(const path & p);
file_status symlink_status(const path & p, std::error_code & ec) noexcept;

#if 0
path system_complete(const path & p);
path system_complete(const path & p, std::error_code & ec);
#endif

path temp_directory_path();
path temp_directory_path(std::error_code & ec);

inline bool status_known(file_status s) noexcept
	{ return (s.type() != file_type::none); }

inline bool exists(file_status s) noexcept
	{ return (status_known(s) && s.type() != file_type::not_found); }

inline bool is_regular_file(file_status s) noexcept
	{ return (s.type() == file_type::regular); }

inline bool is_symlink(file_status s) noexcept
	{ return (s.type() == file_type::symlink); }

inline bool is_block_file(file_status s) noexcept
	{ return (s.type() == file_type::block); }

inline bool is_character_file(file_status s) noexcept
	{ return (s.type() == file_type::character); }

inline bool is_directory(file_status s) noexcept
	{ return (s.type() == file_type::directory); }

inline bool is_fifo(file_status s) noexcept
	{ return (s.type() == file_type::fifo); }

inline bool is_other(file_status s) noexcept
	{ return (  exists(s)
	         && !is_regular_file(s)
	         && !is_directory(s)
	         && !is_symlink(s)); }

inline bool is_socket(file_status s) noexcept
	{ return (s.type() == file_type::socket); }


} // inline namespace v1
} // namespace filesystem

#endif // GUARD_FS_FUNCS_H
