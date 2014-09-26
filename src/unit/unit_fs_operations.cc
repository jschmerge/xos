#include "../filesystem/fs_operations.h"
#include "../filesystem/file_status.h"
#include "../filesystem/path.h"

#include <unistd.h>

#include <string>
#include <functional>

#include "cppunit-header.h"

namespace fs = filesystem::v1;

class Test_fs_operations : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(Test_fs_operations);
	CPPUNIT_TEST(get_current_path);
	CPPUNIT_TEST(set_current_path);
	CPPUNIT_TEST(create_directories);
	CPPUNIT_TEST(status);
	CPPUNIT_TEST(symlink_status);
	CPPUNIT_TEST(temp_directory_path);
	CPPUNIT_TEST_SUITE_END();

 public:
	Test_fs_operations()
	 : file("/etc/passwd")
	 , nonexistent_file("/tmp/badpath")
	 , non_accessible_file("/root/badpath")
		{ }

	void setUp()
	{
		savedCWD = fs::current_path();
	}

	void tearDown()
	{
		// restore the old CWD
		fs::current_path(savedCWD);

		if (config::verbose)
			std::cout << "\n-> RESTORING CWD: " << fs::current_path().c_str()
			          << std::endl;
	}

 protected:
	const fs::path file;
	const fs::path nonexistent_file;
	const fs::path non_accessible_file;

	fs::path savedCWD;

	void test_fs_exception_thrown(std::function<void(void)> f,
	                              fs::path target)
	{
		bool caught = false;
		try {
			try {
				f();
			} catch (fs::filesystem_error & e) {
				CPPUNIT_ASSERT(e.path1() == target);
				CPPUNIT_ASSERT(e.code());
				if (config::verbose)
					std::cout << "Caught exception: " << e.what() << std::endl;
				caught = true;
			} catch (...) {
				throw;
			}
		} catch (std::exception & e) {
			if (config::verbose)
				std::cout << "Caught exception: " << e.what() << std::endl;
			throw;
		} catch (...) {
			if (config::verbose)
				std::cout << "Caught unknown exception!" << std::endl;
			throw;
		}

		CPPUNIT_ASSERT(caught);
	}

	void get_current_path()
	{
// XXX - How do we get this to fail?
		fs::path p = fs::current_path();

		if (config::verbose)
			std::cout << "\nCurrent path = '" << p.c_str() << "'" << std::endl;
	}

	void set_current_path()
	{
		test_fs_exception_thrown(
			[&]() { fs::current_path(non_accessible_file); },
			non_accessible_file);

		CPPUNIT_ASSERT_NO_THROW(fs::current_path(fs::temp_directory_path()));
	}

	void create_directories()
	{
		bool rc = false;
		std::error_code ec;
		fs::path p{"/usr/local/bin"};
		rc = fs::create_directories(p, ec);
		if (ec)
			std::cerr << "Error on /usr/local path = "
			          << ec.message() << std::endl;
		CPPUNIT_ASSERT(!ec);
		CPPUNIT_ASSERT(rc == false);

		ec.clear();
		fs::path tmp{"/tmp/foo" + std::to_string(getpid()) + "/bar"};
		rc = fs::create_directories(tmp, ec);
		if (ec)
			std::cerr << "Error on temp path = " << ec.message() << std::endl;
		CPPUNIT_ASSERT(!ec);
		CPPUNIT_ASSERT(rc == true);
	}

	void status()
	{
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

	void symlink_status()
	{
		fs::file_status s;

		CPPUNIT_ASSERT_NO_THROW(s = fs::symlink_status(file));
		CPPUNIT_ASSERT(s.type() == fs::file_type::regular);

		CPPUNIT_ASSERT_NO_THROW(s = fs::symlink_status(nonexistent_file));
		CPPUNIT_ASSERT(s.type() == fs::file_type::not_found);

		test_fs_exception_thrown(
			[&]() {s = fs::symlink_status(non_accessible_file); },
			non_accessible_file);

		std::error_code ec;
		s = fs::file_status{};
		CPPUNIT_ASSERT_NO_THROW(s = fs::symlink_status(file, ec));
		CPPUNIT_ASSERT(!ec && s.type() == fs::file_type::regular);

		CPPUNIT_ASSERT_NO_THROW(s = fs::symlink_status(nonexistent_file, ec));
		CPPUNIT_ASSERT(!ec && s.type() == fs::file_type::not_found);

		s = fs::file_status{};
		CPPUNIT_ASSERT_NO_THROW(
			s = fs::symlink_status(non_accessible_file, ec));
		CPPUNIT_ASSERT(ec && s.type() == fs::file_type::none);
	}

	void temp_directory_path()
	{
		fs::path p = fs::temp_directory_path();
//		std::cout << "default temporary directory = "
//		          << p.c_str() << std::endl;

		CPPUNIT_ASSERT(p == "/tmp");
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION(Test_fs_operations);
