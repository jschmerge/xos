
#include "../filesystem/file_status.h"

#include "cppunit-header.h"

class Test_file_status : public CppUnit::TestFixture
{
	typedef filesystem::file_status file_status;
	typedef filesystem::file_type file_type;
	typedef filesystem::perms perms;

	CPPUNIT_TEST_SUITE(Test_file_status);
	CPPUNIT_TEST(constructors);
	CPPUNIT_TEST(assignments);
	CPPUNIT_TEST(modifiers);
	CPPUNIT_TEST_SUITE_END();

	const perms rw_r__r__ = (perms::owner_read | perms::owner_write |
	                         perms::group_read | perms::others_read);
 protected:
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
			CPPUNIT_ASSERT(  copied.type() == file_type::regular
			              && copied.permissions() == perms::unknown);
		}
		{
			file_status copied(with_type_and_perms);
			CPPUNIT_ASSERT(  copied.type() == file_type::regular
			              && copied.permissions() == rw_r__r__);

			file_status moved(std::move(with_type_and_perms));
			CPPUNIT_ASSERT(  copied.type() == file_type::regular
			              && copied.permissions() == rw_r__r__);
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
