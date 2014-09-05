#include "../filesystem/filesystem"

#include "cppunit-header.h"

class Test_filesystem_error : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(Test_filesystem_error);
	CPPUNIT_TEST(constructor_test);
	CPPUNIT_TEST_SUITE_END();

 protected:
	void constructor_test()
	{
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION(Test_filesystem_error);
