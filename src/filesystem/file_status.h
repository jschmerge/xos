#ifndef GUARD_FILE_STATUS_H
#define GUARD_FILE_STATUS_H 1

#include "utility/bitmask_operators.h"

namespace filesystem {
inline namespace v1 {

enum class file_type
{
	not_found = -1,
	none = 0,
	regular = 1,
	directory = 2,
	symlink = 3,
	block = 4,
	character = 5,
	fifo = 6,
	socket = 7,
	unknown = 8,
};

enum class perms : unsigned int
{
	none             = 0,

	owner_read       = 00400,
	owner_write      = 00200,
	owner_exec       = 00100,

	group_read       = 00040,
	group_write      = 00020,
	group_exec       = 00010,

	others_read      = 00040,
	others_write     = 00020,
	others_exec      = 00010,

	set_uid          = 04000,
	set_gid          = 02000,
	sticky_bit       = 01000,

	owner_all        = owner_read | owner_write | owner_exec,
	group_all        = group_read | group_write | group_exec,
	others_all       = others_read | others_write | others_exec,
	all              = owner_all | group_all | others_all,
	mask             = all | set_uid | set_gid | sticky_bit,

	unknown          = 0xffff,

	add_perms        = 0x10000,
	remove_perms     = 0x20000,

	add_remove_mask  = 0x30000,

	resolve_symlinks = 0x40000,
};

DEFINE_BITMASK_OPERATORS(perms, unsigned int);

class file_status
{
 public:
	explicit file_status(file_type t = file_type::none,
	                     perms p = perms::unknown) noexcept
	  : m_type(t) , m_permissions(p) { }

	file_status(const file_status &) noexcept = default;

	file_status(file_status &&) noexcept = default;

	~file_status() = default;

	file_status& operator = (const file_status &) noexcept = default;

	file_status& operator = (file_status &&) noexcept = default;

	// observers
	file_type type() const noexcept { return m_type; }

	perms permissions() const noexcept { return m_permissions; }

	// modifiers
	void type(file_type t) noexcept { m_type = t; }

	void permissions(perms p) noexcept { m_permissions = p; }

 private:
	file_type m_type;
	perms m_permissions;
};

} // inline namespace v1
} // namespace filesystem

#endif // GUARD_FILE_STATUS_H
