
#include "../filesystem/path.h"
#include "../filesystem/path_iterator.h"

#include <cstdio>
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <random>

#include "cppunit-header.h"

class Test_Path_Iterator : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(Test_Path_Iterator);
	CPPUNIT_TEST(testSomething);
	CPPUNIT_TEST_SUITE_END();

 protected:
	void testSomething()
	{
		using namespace filesystem;

		std::vector<std::string> values {
			{ "" },
			{ "/" },
			{ "/////" },
			{ "foo" },
			{ "foo/" },
			{ "foo/////" },
			{ "/foo" },
			{ "/foo/" },
			{ "foo/bar" },
			{ "/foo/bar" },
			{ "/foo/bar/" },
		};

		putchar('\n');
		for (auto & s : values)
		{
			std::cout << "Iterating through '" << s << "'\n";
			path p(s);

			path_iterator i(&p, 0);
			const filesystem::path_iterator end(&p, path_iterator::npos);

			while (i != end)
			{
				++i;
			}
			std::cout << "--------------------------------\n";
		}
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION(Test_Path_Iterator);
