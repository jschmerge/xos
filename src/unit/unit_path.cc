#include "../filesystem/filesystem"

#include "cppunit-header.h"

#include <cstdio>
#include <cstring>
#include <iostream>

class Test_Path : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(Test_Path);
	CPPUNIT_TEST(defaultConstructor);
	CPPUNIT_TEST(valueConstructor);
	CPPUNIT_TEST_SUITE_END();

 protected:
	void defaultConstructor()
	{
		filesystem::path p;
		CPPUNIT_ASSERT(p.empty());
		CPPUNIT_ASSERT(p.native() == "");
		CPPUNIT_ASSERT(*p.c_str() == '\0');
		std::string s = p;

		CPPUNIT_ASSERT(s.empty());
		CPPUNIT_ASSERT(s == "");
		CPPUNIT_ASSERT(*s.c_str() == '\0');
	}

	void valueConstructor()
	{
		{
			const char value[] = "/foo/bar";
			filesystem::path p1(value);
			CPPUNIT_ASSERT(! p1.empty());
			CPPUNIT_ASSERT(p1.native() == value);
			CPPUNIT_ASSERT(strcmp(p1.c_str(), value) == 0);

			std::string val2(value);
			filesystem::path p2(val2);
			CPPUNIT_ASSERT(! p2.empty());
			CPPUNIT_ASSERT(p2.native() == value);
			CPPUNIT_ASSERT(strcmp(p2.c_str(), value) == 0);
		}

		{
			const char * value = "/foo/bar";
			filesystem::path p1(value);
			CPPUNIT_ASSERT(! p1.empty());
			CPPUNIT_ASSERT(p1.native() == value);
			CPPUNIT_ASSERT(strcmp(p1.c_str(), value) == 0);

			std::string val2(value);
			filesystem::path p2(val2);
			CPPUNIT_ASSERT(! p2.empty());
			CPPUNIT_ASSERT(p2.native() == value);
			CPPUNIT_ASSERT(strcmp(p2.c_str(), value) == 0);
		}
	}

};

CPPUNIT_TEST_SUITE_REGISTRATION(Test_Path);
