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
#if 0
		const char value[] = "/foo/bar";
		filesystem::path p(value);
		CPPUNIT_ASSERT(! p.empty());
#if 0
//		fprintf(stderr, "---> \"%s\" \"%s\"\n", p.native().c_str(),
//		        value);

		std::cerr << "'" << p.native() << "' '" << value << "'" << std::endl;
		//CPPUNIT_ASSERT(p.native() == value);
		CPPUNIT_ASSERT(strcmp(p.c_str(), value) == 0);

//		std::string s(value);
		std::string s = p;
		std::cerr << s << ' ' << s.length() << ' '
		          << value << ' ' << strlen(value) << std::endl;
		CPPUNIT_ASSERT(s == value);
//
//		CPPUNIT_ASSERT(! s.empty());
//		CPPUNIT_ASSERT(s == value);
//		CPPUNIT_ASSERT(strcmp(s.c_str(), value) == 0);
#endif
	}

};

CPPUNIT_TEST_SUITE_REGISTRATION(Test_Path);
