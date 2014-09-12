#include "../filesystem/filesystem"

#include "cppunit-header.h"

namespace fs = filesystem;
class Test_filesystem_error : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(Test_filesystem_error);
	CPPUNIT_TEST(constructor_test);
	CPPUNIT_TEST_SUITE_END();

 protected:
	void constructor_test()
	{
		fs::path p1("/foo/bar");
		fs::path p2("/blah/oops");
		fs::filesystem_error fe0("bad0", std::error_code());
		fs::filesystem_error fe1("bad1", p1, std::error_code());
		fs::filesystem_error fe2("bad1", p1, p2, std::error_code());

		CPPUNIT_ASSERT(strcmp(fe1.path1().c_str(), p1.c_str()) == 0);
		CPPUNIT_ASSERT(fe1.path2().empty());


		CPPUNIT_ASSERT(strcmp(fe2.path1().c_str(), p1.c_str()) == 0);
		CPPUNIT_ASSERT(strcmp(fe2.path2().c_str(), p2.c_str()) == 0);

		if (config::verbose)
		{
			printf("filesystem_error (0 path) what: %s\n", fe0.what());
			printf("filesystem_error (1 path) what: %s\n", fe1.what());
			printf("filesystem_error (2 path) what: %s\n", fe2.what());
		}
		
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION(Test_filesystem_error);
