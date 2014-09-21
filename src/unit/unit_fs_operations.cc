#include "../filesystem/fs_operations.h"
#include "../filesystem/file_status.h"
#include "../filesystem/path.h"

#include <functional>

#include "cppunit-header.h"

namespace fs = filesystem::v1;

class Test_fs_operations : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(Test_fs_operations);
	CPPUNIT_TEST(get_current_path);
	CPPUNIT_TEST(status);
	CPPUNIT_TEST_SUITE_END();

 protected:
	void test_fs_exception_thrown(std::function<void(void)> f,
	                              fs::path target)
	{
		bool caught = false;
		try {
			f();
		} catch (fs::filesystem_error & e)
		{
			CPPUNIT_ASSERT(e.path1() == target);
			CPPUNIT_ASSERT(e.code());
			caught = true;
		} catch (...)
		{
			throw;
		}

		CPPUNIT_ASSERT(caught);
	}

	void get_current_path()
	{
// XXX - How do we get this to fail?
		fs::path p = fs::current_path();
		putchar('\n');
		std::cout << "Current path = '" << p.c_str() << "'" << std::endl;
	}

	void status()
	{
		const fs::path file("/etc/passwd");
		const fs::path nonexistent_file("/tmp/badpath");
		const fs::path non_accessible_file("/root/badpath");

		fs::file_status s;

		CPPUNIT_ASSERT_NO_THROW(s = fs::status(file));
		CPPUNIT_ASSERT(s.type() == fs::file_type::regular);

		CPPUNIT_ASSERT_NO_THROW(s = fs::status(nonexistent_file));
		CPPUNIT_ASSERT(s.type() == fs::file_type::not_found);

		test_fs_exception_thrown(
			[&]() {s = fs::status(non_accessible_file); },
			non_accessible_file);

		std::error_code ec;
		s = fs::file_status{};
		CPPUNIT_ASSERT_NO_THROW(s = fs::status(file, ec));
		CPPUNIT_ASSERT(!ec && s.type() == fs::file_type::regular);

		CPPUNIT_ASSERT_NO_THROW(s = fs::status(nonexistent_file, ec));
		CPPUNIT_ASSERT(!ec && s.type() == fs::file_type::not_found);

		s = fs::file_status{};
		CPPUNIT_ASSERT_NO_THROW(s = fs::status(non_accessible_file, ec));
		CPPUNIT_ASSERT(ec && s.type() == fs::file_type::none);
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION(Test_fs_operations);
