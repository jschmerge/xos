#include "time/timeutil.h"

#include <iostream>
#include <iomanip>

#include "cppunit-header.h"
/*
	realtime = CLOCK_REALTIME,
	course_realtime = CLOCK_REALTIME_COARSE,
	monotonic = CLOCK_MONOTONIC,
	course_monotonic = CLOCK_MONOTONIC_COARSE,
	raw_monotonic = CLOCK_MONOTONIC_RAW,
	bootime = CLOCK_BOOTTIME,
	process_cputime = CLOCK_PROCESS_CPUTIME_ID,
	thread_cputime = CLOCK_THREAD_CPUTIME_ID,
*/

class Test_timeutil : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(Test_timeutil);
	CPPUNIT_TEST(clock_info);
	CPPUNIT_TEST(clock_time);
	CPPUNIT_TEST(duration_manipulation);
	CPPUNIT_TEST_SUITE_END();

	void clock_info()
	{
		using std::chrono::seconds;
		using std::chrono::duration_cast;
		if (config::verbose) std::cout << "\nClock resolotions:\n";

		std::string name;
		std::chrono::duration<double> d;

		name = posix_clock<clock_source::realtime>::name();
		d = posix_clock<clock_source::realtime>::resolution();
		CPPUNIT_ASSERT(d < seconds(1));
		if (config::verbose)
			std::cout << std::setw(20) << name << ": "
			          << std::fixed << std::setprecision(9)
			          << d.count() << std::endl;

		name = posix_clock<clock_source::course_realtime>::name();
		d = posix_clock<clock_source::course_realtime>::resolution();
		CPPUNIT_ASSERT(d < seconds(1));
		if (config::verbose)
			std::cout << std::setw(20) << name << ": "
			          << std::fixed << std::setprecision(9)
			          << d.count() << std::endl;

		name = posix_clock<clock_source::monotonic>::name();
		d = posix_clock<clock_source::monotonic>::resolution();
		CPPUNIT_ASSERT(d < seconds(1));
		if (config::verbose)
			std::cout << std::setw(20) << name << ": "
			          << std::fixed << std::setprecision(9)
			          << d.count() << std::endl;

		name = posix_clock<clock_source::course_monotonic>::name();
		d = posix_clock<clock_source::course_monotonic>::resolution();
		CPPUNIT_ASSERT(d < seconds(1));
		if (config::verbose)
			std::cout << std::setw(20) << name << ": "
			          << std::fixed << std::setprecision(9)
			          << d.count() << std::endl;

		name = posix_clock<clock_source::raw_monotonic>::name();
		d = posix_clock<clock_source::raw_monotonic>::resolution();
		CPPUNIT_ASSERT(d < seconds(1));
		if (config::verbose)
			std::cout << std::setw(20) << name << ": "
			          << std::fixed << std::setprecision(9)
			          << d.count() << std::endl;

		name = posix_clock<clock_source::boot_time>::name();
		d = posix_clock<clock_source::boot_time>::resolution();
		CPPUNIT_ASSERT(d < seconds(1));
		if (config::verbose)
			std::cout << std::setw(20) << name << ": "
			          << std::fixed << std::setprecision(9)
			          << d.count() << std::endl;

		name = posix_clock<clock_source::process_cpu_time>::name();
		d = posix_clock<clock_source::process_cpu_time>::resolution();
		CPPUNIT_ASSERT(d < seconds(1));
		if (config::verbose)
			std::cout << std::setw(20) << name << ": "
			          << std::fixed << std::setprecision(9)
			          << d.count() << std::endl;

		name = posix_clock<clock_source::thread_cpu_time>::name();
		d = posix_clock<clock_source::thread_cpu_time>::resolution();
		CPPUNIT_ASSERT(d < seconds(1));
		if (config::verbose)
			std::cout << std::setw(20) << name << ": "
			          << std::fixed << std::setprecision(9)
			          << d.count() << std::endl;
	}

	void clock_time()
	{
		using clk = posix_clock<clock_source::realtime>;

		struct timespec ts1, ts2;
		auto t = clk::now();
		clock_gettime(CLOCK_REALTIME, &ts1);

		auto u = to_timepoint<clk, clk::duration>(ts1);


		CPPUNIT_ASSERT((u - t) < std::chrono::microseconds(500));

		ts2 = to_timespec(t);

		printf("%lu.%09lu %lu.%09lu\n",
		       ts1.tv_sec, ts1.tv_nsec,
		       ts2.tv_sec, ts2.tv_nsec);
	}

	void duration_manipulation()
	{
		struct timespec ts1 = { 1, 500000000};
		struct timespec ts2 = { 2, 300000000};

		auto d1 = to_duration<std::chrono::nanoseconds>(ts1);
		auto d2 = to_duration<std::chrono::nanoseconds>(ts2);

		auto difference = d2 - d1;

		CPPUNIT_ASSERT(difference.count() == 800000000);
		ts1 = to_timespec(difference);
		CPPUNIT_ASSERT(ts1.tv_nsec == 800000000);
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION(Test_timeutil);
