#include "../filesystem/filesystem"

#include "cppunit-header.h"

class Test_Path : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(Test_Path);
	CPPUNIT_TEST(defaultConstructor);
	CPPUNIT_TEST_SUITE_END();

 protected:
	void defaultConstructor()
	{
		std::experimental::filesystem::path p;
		CPPUNIT_ASSERT(p.empty());
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION(Test_Path);
