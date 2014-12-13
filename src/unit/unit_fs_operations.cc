#include "filesystem/fs_operations.h"
#include "filesystem/directory_iterator.h"
#include "filesystem/file_status.h"
#include "filesystem/path.h"

#include <unistd.h>

#include <cstdlib>
#include <string>
#include <functional>

#include "cppunit-header.h"

namespace fs = filesystem::v1;

class Test_fs_operations : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(Test_fs_operations);
	CPPUNIT_TEST(copy_options_bitmask_operators);
	CPPUNIT_TEST(absolute);
	CPPUNIT_TEST(canonical);
	CPPUNIT_TEST(copy_file);
	CPPUNIT_TEST(copy_symlink);
	CPPUNIT_TEST(create_directories);
	CPPUNIT_TEST(create_directory);
	CPPUNIT_TEST(get_current_path);
	CPPUNIT_TEST(set_current_path);
	CPPUNIT_TEST(is_empty);
	CPPUNIT_TEST(rename);
	CPPUNIT_TEST(space);
	CPPUNIT_TEST(status);
	CPPUNIT_TEST(symlink_status);
	CPPUNIT_TEST(last_write_time);
	CPPUNIT_TEST(temp_directory_path);
	CPPUNIT_TEST(remove_all);
	CPPUNIT_TEST_SUITE_END();

 public:
	Test_fs_operations()
	 : file("/etc/passwd")
	 , nonexistent_file("/tmp/badpath")
	 , non_accessible_file("/root/badpath")
		{ }

	void setUp()
	{
		saved_cwd = fs::current_path();

		const char * tmp_ptr = getenv("TMPDIR");
		if (tmp_ptr != nullptr)
			saved_tmp_env_var = tmp_ptr;

		fs::path tmp = fs::temp_directory_path();

		tmp /= "unittest.";
		tmp += std::to_string(getpid());

		CPPUNIT_ASSERT(setenv("TMPDIR", tmp.c_str(), 1) == 0);

		fs::create_directories(tmp);
	}

	void tearDown()
	{
		fs::remove_all(fs::temp_directory_path());

		// restore the old CWD
		fs::current_path(saved_cwd);

		if (config::verbose)
			std::cout << "\n-> RESTORING CWD: " << fs::current_path().c_str()
			          << std::endl;
		if (!saved_tmp_env_var.empty())
			CPPUNIT_ASSERT(setenv("TMPDIR", saved_tmp_env_var.c_str(), 1) == 0);
		else
			CPPUNIT_ASSERT(unsetenv("TMPDIR") == 0);

		saved_tmp_env_var.clear();
		saved_cwd.clear();
	}

 protected:
	const fs::path file;
	const fs::path nonexistent_file;
	const fs::path non_accessible_file;

	fs::path saved_cwd;
	std::string	saved_tmp_env_var;

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

	void copy_options_bitmask_operators()
	{
		const fs::copy_options full = ~(fs::copy_options::none);
		const fs::copy_options empty = fs::copy_options::none;

		CPPUNIT_ASSERT(full != empty);

		for (auto x : { fs::copy_options::skip_existing,
		                fs::copy_options::overwrite_existing,
		                fs::copy_options::update_existing,
		                fs::copy_options::existing_entry_group,
		                fs::copy_options::recursive,
		                fs::copy_options::copy_symlinks,
		                fs::copy_options::skip_symlinks,
		                fs::copy_options::symlink_group,
		                fs::copy_options::directories_only,
		                fs::copy_options::create_symlinks,
		                fs::copy_options::create_hard_links,
		                fs::copy_options::directory_group } )
		{
			CPPUNIT_ASSERT((x & ~x) == empty);
			CPPUNIT_ASSERT(~x != full);
			CPPUNIT_ASSERT(is_set(x, x));
			CPPUNIT_ASSERT(!is_set(~x, x));

			CPPUNIT_ASSERT((empty & x) == fs::copy_options::none);
			CPPUNIT_ASSERT((empty | x) == x);
			CPPUNIT_ASSERT((empty ^ x) == x);

			CPPUNIT_ASSERT((full & x) == x);
			CPPUNIT_ASSERT((full | x) == full);
			CPPUNIT_ASSERT((full ^ x) == ~x);

			fs::copy_options tmp;

			tmp = full;
			tmp &= x;
			CPPUNIT_ASSERT(tmp == (full & x));

			tmp = full;
			tmp |= x;
			CPPUNIT_ASSERT(tmp == (full | x));

			tmp = full;
			tmp ^= x;
			CPPUNIT_ASSERT(tmp == (full ^ x));

			tmp = empty;
			tmp &= x;
			CPPUNIT_ASSERT(tmp == (empty & x));

			tmp = empty;
			tmp |= x;
			CPPUNIT_ASSERT(tmp == (empty | x));

			tmp = empty;
			tmp ^= x;
			CPPUNIT_ASSERT(tmp == (empty ^ x));

		}
	}

	void absolute()
	{
		fs::path full_path{"/usr/local"};
		fs::path base_path{"/usr"};
		fs::path a{"local"};
		fs::path b{"/usr/local"};

		CPPUNIT_ASSERT(fs::absolute(a, base_path) == full_path);
		CPPUNIT_ASSERT(fs::absolute(b, base_path) == full_path);
		
	}

	void canonical()
	{
		fs::create_directory_symlink("/root/.ssh", "/tmp/badlink");
		fs::path bad_link{"/tmp/badlink"};
		fs::path bad_path{"/badpath"};
		fs::path full_path{"/usr/local"};
		fs::path base_path{"/usr"};
		fs::path a1{"local/../local/bin/.."};
		fs::path a2{"/usr/local/../../../../usr/./local"};

		CPPUNIT_ASSERT_THROW(fs::canonical(bad_link), fs::filesystem_error);
		CPPUNIT_ASSERT_THROW(fs::canonical(bad_path), fs::filesystem_error);

		CPPUNIT_ASSERT(fs::canonical(a1, base_path) == full_path);
		CPPUNIT_ASSERT(fs::canonical(a2, base_path) == full_path);

		std::error_code ec;
		current_path(base_path, ec);
		CPPUNIT_ASSERT(!ec);
		CPPUNIT_ASSERT(fs::canonical(a1, ec) == full_path);
		CPPUNIT_ASSERT(!ec);
		CPPUNIT_ASSERT(fs::canonical(a2, ec) == full_path);
		CPPUNIT_ASSERT(!ec);
		fs::remove(bad_link);
	}

	void copy_file()
	{
		using CO = fs::copy_options;
		std::vector<fs::copy_options> bad_options{
			CO::skip_existing | CO::overwrite_existing,
			CO::skip_existing | CO::update_existing,
			CO::overwrite_existing | CO::update_existing,
			CO::skip_existing | CO::overwrite_existing | CO::update_existing,
			CO::copy_symlinks | CO::skip_symlinks,

			CO::directories_only | CO::create_symlinks,
			CO::directories_only | CO::create_hard_links,
			CO::create_symlinks | CO::create_hard_links,
			CO::directories_only | CO::create_symlinks | CO::create_hard_links,
		};

		for (const auto & o : bad_options)
		{
			CPPUNIT_ASSERT_THROW(fs::copy_file("a", "b", o),
			                     fs::filesystem_error);
		}

		std::error_code ec;
		CPPUNIT_ASSERT(!fs::copy_file("/badpath", "b", ec));
		CPPUNIT_ASSERT(ec);

		CPPUNIT_ASSERT(fs::copy_file("/etc/fstab", "/tmp/xxx",
		                             fs::copy_options::overwrite_existing));
		CPPUNIT_ASSERT_THROW(fs::copy_file("/etc/fstab", "/tmp/xxx"),
		                     fs::filesystem_error);

		CPPUNIT_ASSERT(fs::remove("/tmp/xxx"));
	}

	void copy_symlink()
	{
		fs::path file{"/etc/fstab"};
		fs::path link1{fs::temp_directory_path() / "link_orig"};
		fs::path link2{fs::temp_directory_path() / "link_copy"};
		CPPUNIT_ASSERT_NO_THROW(fs::create_symlink(file, link1));
		CPPUNIT_ASSERT_NO_THROW(fs::copy_symlink(link1, link2));

		CPPUNIT_ASSERT(fs::read_symlink(link1) == fs::read_symlink(link2));
	}

	void create_directories()
	{
		bool rc = false;
		std::error_code ec;
		fs::path p{"/usr/local/bin"};
		rc = fs::create_directories(p, ec);
		CPPUNIT_ASSERT(!ec);
		CPPUNIT_ASSERT(rc == false);

		ec.clear();
		fs::path tmp{fs::temp_directory_path() / "bar"};
		rc = fs::create_directories(tmp, ec);
		CPPUNIT_ASSERT(!ec);
		CPPUNIT_ASSERT(rc == true);
		fs::path bad{"/root/subdir/bad"};
		CPPUNIT_ASSERT_THROW(fs::create_directories(bad),
		                     fs::filesystem_error);
	}

	void create_directory()
	{
		fs::path dir(fs::temp_directory_path() / "created_dir");
		fs::path dir2(fs::temp_directory_path() / "copied_dir");
		fs::path file(fs::temp_directory_path() / "created_dir/file");
		CPPUNIT_ASSERT_NO_THROW(fs::create_directory(dir));
		CPPUNIT_ASSERT(fs::is_directory(dir));
		CPPUNIT_ASSERT_NO_THROW(fs::create_directory(dir));
		CPPUNIT_ASSERT(fs::copy_file("/etc/fstab", file));
		CPPUNIT_ASSERT_THROW(fs::create_directory(file), fs::filesystem_error);

		CPPUNIT_ASSERT_NO_THROW(fs::permissions(dir, fs::perms::owner_all));
		CPPUNIT_ASSERT(fs::status(dir).permissions() == fs::perms::owner_all);
		CPPUNIT_ASSERT(fs::create_directory(dir2, dir));
		CPPUNIT_ASSERT(fs::status(dir2).permissions() == fs::perms::owner_all);
	}

	void get_current_path()
	{
#if 0
// XXX - How do we get this to fail?
		fs::path p = fs::current_path();

		if (config::verbose)
			std::cout << "\nCurrent path = '" << p.c_str() << "'" << std::endl;
#endif
		fs::path p{fs::temp_directory_path() / "dir"};
		CPPUNIT_ASSERT_NO_THROW(fs::create_directory(p));
		CPPUNIT_ASSERT_NO_THROW(fs::current_path(p));
		CPPUNIT_ASSERT(fs::current_path() == p);
		CPPUNIT_ASSERT_NO_THROW(fs::remove(p));
		CPPUNIT_ASSERT_THROW(fs::current_path(), fs::filesystem_error);
	}

	void set_current_path()
	{
		test_fs_exception_thrown(
			[&]() { fs::current_path(non_accessible_file); },
			non_accessible_file);

		CPPUNIT_ASSERT_NO_THROW(fs::current_path(fs::temp_directory_path()));
	}

	void is_empty()
	{
		// Test directory
		fs::path p = fs::temp_directory_path() / "emptydir";
		std::error_code ec;
		try { fs::remove_all(p); } catch (...) { }
		fs::create_directory(p, ec);
		CPPUNIT_ASSERT(!ec);
		CPPUNIT_ASSERT(fs::is_empty(p));
		CPPUNIT_ASSERT(fs::create_directories(p / "subdir"));
		CPPUNIT_ASSERT(!fs::is_empty(p));

		// Test File
		fs::path q = p / "foo";
		CPPUNIT_ASSERT(fs::copy_file("/etc/fstab", q));
		CPPUNIT_ASSERT(!fs::is_empty(q));
		CPPUNIT_ASSERT_NO_THROW(fs::resize_file(q, 0));
		CPPUNIT_ASSERT(fs::is_empty(q));
		CPPUNIT_ASSERT(fs::remove_all(p) != 0);
	}

	void rename()
	{
		fs::path p = fs::temp_directory_path() / "foo";
		fs::path p2 = p;
		p2.replace_filename("bar");

		CPPUNIT_ASSERT(fs::copy_file("/etc/fstab", p));
		CPPUNIT_ASSERT(fs::exists(p));
		CPPUNIT_ASSERT(!fs::exists(p2));

		CPPUNIT_ASSERT_NO_THROW(fs::rename(p, p2));
		CPPUNIT_ASSERT(!fs::exists(p));
		CPPUNIT_ASSERT(fs::exists(p2));

		CPPUNIT_ASSERT_THROW(fs::rename(nonexistent_file, p2),
		                     fs::filesystem_error);
	}

	void space()
	{
		fs::space_info s;
		fs::path goodpath{fs::temp_directory_path()};
		fs::path badpath{"/badpath"};

		CPPUNIT_ASSERT_NO_THROW(s = fs::space(fs::temp_directory_path()));
		CPPUNIT_ASSERT(  s.capacity > 0
		              && s.capacity != static_cast<uintmax_t>(-1));
		CPPUNIT_ASSERT(  s.available > 0
		              && s.available != static_cast<uintmax_t>(-1));
		CPPUNIT_ASSERT(  s.free > 0
		              && s.free != static_cast<uintmax_t>(-1));
		// not sure what else we can test here
		CPPUNIT_ASSERT(s.capacity >= s.available);
		CPPUNIT_ASSERT(s.capacity >= s.free);

		CPPUNIT_ASSERT_THROW(fs::space(badpath), fs::filesystem_error);
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

	void last_write_time()
	{
		fs::path p{"/etc/passwd"};
		auto t0 = fs::file_time_type::min();
		auto tp = fs::last_write_time(p);
		CPPUNIT_ASSERT(tp > t0);

		fs::path q{fs::temp_directory_path() / "bad"};
		CPPUNIT_ASSERT_THROW(fs::last_write_time(q), fs::filesystem_error);
	}

	void remove_all()
	{
		std::error_code ec;
		fs::path tmp{fs::temp_directory_path() / "foo"};
		CPPUNIT_ASSERT(fs::create_directories(tmp));

		CPPUNIT_ASSERT_NO_THROW(fs::remove_all(tmp, ec));
		CPPUNIT_ASSERT(!ec);
		CPPUNIT_ASSERT(!exists(tmp));

		fs::path longpath(tmp / "bar" / "ugg");
		CPPUNIT_ASSERT(fs::create_directories(longpath));
		CPPUNIT_ASSERT(fs::remove_all(tmp) > 0);
		CPPUNIT_ASSERT(!exists(tmp));

		CPPUNIT_ASSERT(!ec);
		CPPUNIT_ASSERT(!exists(tmp));

	}

	void temp_directory_path()
	{
		fs::path p = fs::temp_directory_path();
//		std::cout << "default temporary directory = "
//		          << p.c_str() << std::endl;

		CPPUNIT_ASSERT(fs::is_directory(p));
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION(Test_fs_operations);
