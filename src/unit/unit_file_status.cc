
#include "filesystem/file_status.h"

#include "cppunit-header.h"

namespace fs = filesystem;

class Test_file_status : public CppUnit::TestFixture
{
	typedef fs::file_status file_status;
	typedef fs::file_type file_type;
	typedef fs::perms perms;

	CPPUNIT_TEST_SUITE(Test_file_status);
	CPPUNIT_TEST(perms_bitmask_operators);
	CPPUNIT_TEST(constructors);
	CPPUNIT_TEST(assignments);
	CPPUNIT_TEST(modifiers);
	CPPUNIT_TEST_SUITE_END();

	const perms rw_r__r__ = (perms::owner_read | perms::owner_write |
	                         perms::group_read | perms::others_read);
 protected:
	void perms_bitmask_operators()
	{
		const fs::perms full = ~(fs::perms::none);
		const fs::perms empty = fs::perms::none;

		CPPUNIT_ASSERT(full != empty);

		for (auto x : { fs::perms::owner_read, fs::perms::owner_write,
		                fs::perms::owner_exec, fs::perms::group_read,
		                fs::perms::group_write, fs::perms::group_exec,
		                fs::perms::others_read, fs::perms::others_write,
		                fs::perms::others_exec, fs::perms::set_uid,
		                fs::perms::set_gid, fs::perms::sticky_bit,
		                fs::perms::owner_all, fs::perms::group_all,
		                fs::perms::others_all, fs::perms::all,
		                fs::perms::mask, fs::perms::unknown,
		                fs::perms::add_perms, fs::perms::remove_perms,
		                fs::perms::add_remove_mask,
		                fs::perms::resolve_symlinks } )
		{
			CPPUNIT_ASSERT((x & ~x) == empty);
			CPPUNIT_ASSERT(~x != full);
			CPPUNIT_ASSERT(is_set(x, x));
			CPPUNIT_ASSERT(!is_set(~x, x));

			CPPUNIT_ASSERT((empty & x) == fs::perms::none);
			CPPUNIT_ASSERT((empty | x) == x);
			CPPUNIT_ASSERT((empty ^ x) == x);

			CPPUNIT_ASSERT((full & x) == x);
			CPPUNIT_ASSERT((full | x) == full);
			CPPUNIT_ASSERT((full ^ x) == ~x);

			fs::perms tmp;

			tmp = full;
			tmp &= x;
			CPPUNIT_ASSERT(tmp == (full & x));

			tmp = full;
			tmp |= x;
			CPPUNIT_ASSERT(tmp == (full | x));

			tmp = full;
			tmp ^= x;
			CPPUNIT_ASSERT(tmp == (full ^ x));

			tmp = empty;
			tmp &= x;
			CPPUNIT_ASSERT(tmp == (empty & x));

			tmp = empty;
			tmp |= x;
			CPPUNIT_ASSERT(tmp == (empty | x));

			tmp = empty;
			tmp ^= x;
			CPPUNIT_ASSERT(tmp == (empty ^ x));

		}
	}

	void constructors()
	{
		file_status def_constructed;
		file_status with_type(file_type::regular);
		file_status with_type_and_perms(file_type::regular, rw_r__r__);

		CPPUNIT_ASSERT(  def_constructed.type() == file_type::none
		              && def_constructed.permissions() == perms::unknown);
		CPPUNIT_ASSERT(  with_type.type() == file_type::regular
		              && with_type.permissions() == perms::unknown);
		CPPUNIT_ASSERT(  with_type_and_perms.type() == file_type::regular
		              && with_type_and_perms.permissions() == rw_r__r__);

		{
			file_status copied(with_type);
			CPPUNIT_ASSERT(  copied.type() == file_type::regular
			              && copied.permissions() == perms::unknown);

			file_status moved(std::move(with_type));
			CPPUNIT_ASSERT(  moved.type() == file_type::regular
			              && moved.permissions() == perms::unknown);
		}
		{
			file_status copied(with_type_and_perms);
			CPPUNIT_ASSERT(  copied.type() == file_type::regular
			              && copied.permissions() == rw_r__r__);

			file_status moved(std::move(with_type_and_perms));
			CPPUNIT_ASSERT(  moved.type() == file_type::regular
			              && moved.permissions() == rw_r__r__);
		}
	}

	void assignments()
	{
		file_status with_type_and_perms(file_type::regular, rw_r__r__);

		file_status x, y;

		CPPUNIT_ASSERT(  x.type() == file_type::none
		              && x.permissions() == perms::unknown);
		CPPUNIT_ASSERT(  y.type() == file_type::none
		              && y.permissions() == perms::unknown);

		x = with_type_and_perms;
		CPPUNIT_ASSERT(  x.type() == file_type::regular
		              && x.permissions() == rw_r__r__);

		y = std::move(with_type_and_perms);
		CPPUNIT_ASSERT(  y.type() == file_type::regular
		              && y.permissions() == rw_r__r__);
	}

	void modifiers()
	{
		file_status fs;
		CPPUNIT_ASSERT(   fs.type() == file_type::none
		               && fs.permissions() == perms::unknown);
		fs.type(file_type::directory);
		CPPUNIT_ASSERT(   fs.type() == file_type::directory
		               && fs.permissions() == perms::unknown);
		fs.permissions(rw_r__r__);
		CPPUNIT_ASSERT(   fs.type() == file_type::directory
		               && fs.permissions() == rw_r__r__);
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION(Test_file_status);
