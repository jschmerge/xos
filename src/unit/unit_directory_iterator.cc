#include "../filesystem/path.h"
#include "../filesystem/directory_iterator.h"

#include <iostream>

#include "cppunit-header.h"

namespace fs = filesystem::v1;
class Test_directory_iterator : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(Test_directory_iterator);
	CPPUNIT_TEST(constructors);
	CPPUNIT_TEST_SUITE_END();

 protected:
	void constructors()
	{
		CPPUNIT_ASSERT_NO_THROW(fs::directory_iterator i;);
		CPPUNIT_ASSERT_NO_THROW(fs::directory_iterator i("/tmp"););

		putchar('\n');
		for (fs::directory_iterator i = fs::directory_iterator("/tmp");
		     i != end(i); ++i)
		{
			std::cout << i->path().c_str() << std::endl;
		}

		CPPUNIT_ASSERT_THROW(fs::directory_iterator i("/root");,
		                     fs::filesystem_error);
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION(Test_directory_iterator);
