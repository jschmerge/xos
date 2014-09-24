#include "../filesystem/path.h"
#include "../filesystem/directory_iterator.h"

#include <cstdio>
#include <iostream>
#include <set>

#include "cppunit-header.h"

namespace fs = filesystem::v1;

class Test_directory_iterator : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(Test_directory_iterator);
	CPPUNIT_TEST(constructors);
	CPPUNIT_TEST(iteration);
	CPPUNIT_TEST_SUITE_END();

 protected:
	void constructors()
	{
		std::error_code ec;
		fs::directory_iterator empty;

		CPPUNIT_ASSERT(empty == end(fs::directory_iterator()));

		CPPUNIT_ASSERT_NO_THROW(fs::directory_iterator i;);
		CPPUNIT_ASSERT_NO_THROW(fs::directory_iterator i(fs::path{"/tmp"}););

		CPPUNIT_ASSERT_THROW({fs::directory_iterator j(fs::path("/foo"));},
		                     fs::filesystem_error);

		CPPUNIT_ASSERT_THROW({fs::directory_iterator j(fs::path{"/root"});},
		                     fs::filesystem_error);

		ec.clear();
		CPPUNIT_ASSERT_NO_THROW(
			fs::directory_iterator j(fs::path("/foo"), ec););
		CPPUNIT_ASSERT(ec);
		               
		ec.clear();
		CPPUNIT_ASSERT_NO_THROW(
			fs::directory_iterator j(fs::path("/foo"), ec););
		CPPUNIT_ASSERT(ec);
	}

	void iteration()
	{
		std::set<fs::path> paths;

		if (config::verbose)
			putchar('\n'); 

		fs::directory_iterator i(fs::path("/tmp"));
		CPPUNIT_ASSERT(i != end(i));

		for (; i != end(i); ++i)
		{
			if (config::verbose)
				std::cout << " entry of /tmp: " << i->path().c_str() << '\n';

			std::set<fs::path>::iterator rv;
			bool inserted = false;

			std::tie(rv, inserted) = paths.insert(*i);

			CPPUNIT_ASSERT(inserted == true);
		}

		i = fs::directory_iterator(fs::path("/tmp"));
		for (auto & j : i)
		{
			CPPUNIT_ASSERT(paths.find(j) != paths.end());
		}
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION(Test_directory_iterator);
