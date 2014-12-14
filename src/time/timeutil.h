#ifndef GUARD_TIMEUTIL_H
#define GUARD_TIMEUTIL_H 1

#include <cinttypes>
#include <cstdio>
#include <chrono>
#include <tuple>
#include <cassert>
#include <system_error>

#include "utility/util.h"

//////////////////////////////////////////////////////////////////////
enum class clock_source : clockid_t
{
	realtime = CLOCK_REALTIME,
	course_realtime = CLOCK_REALTIME_COARSE,
	monotonic = CLOCK_MONOTONIC,
	course_monotonic = CLOCK_MONOTONIC_COARSE,
	raw_monotonic = CLOCK_MONOTONIC_RAW,
	boot_time = CLOCK_BOOTTIME,
	process_cpu_time = CLOCK_PROCESS_CPUTIME_ID,
	thread_cpu_time = CLOCK_THREAD_CPUTIME_ID,
};

//////////////////////////////////////////////////////////////////////
template<clock_source SOURCE>
struct posix_clock
{
	typedef std::chrono::nanoseconds duration;
	typedef duration::rep rep;
	typedef duration::period period;
	typedef std::chrono::time_point<posix_clock, duration> time_point;

	static_assert(posix_clock::duration::min() <
	                posix_clock::duration::zero(),
	              "a clock's minimum duration cannot be less than its epoch");

	static constexpr
	bool is_steady = (  (SOURCE == clock_source::monotonic)
	                 || (SOURCE == clock_source::course_monotonic)
	                 || (SOURCE == clock_source::raw_monotonic));

	static time_point now();
	static duration resolution();

	static const clock_source source = SOURCE;

};

//////////////////////////////////////////////////////////////////////
template <class CLK, class DUR>
struct timespec
to_timespec(const std::chrono::time_point<CLK, DUR> & t) noexcept
{
	using std::chrono::seconds;
	using std::chrono::nanoseconds;
	using std::chrono::duration_cast;
	
	const int64_t oneBillion = 1000000000;
	struct timespec ts;
	auto dur = t.time_since_epoch();
	ts.tv_sec = time_t(duration_cast<seconds>(dur).count());
	ts.tv_nsec = (duration_cast<nanoseconds>(dur).count() % oneBillion);

	return ts;
}

//////////////////////////////////////////////////////////////////////
template <class DURATION>
struct timespec to_timespec(const DURATION & d) noexcept
{
	using std::chrono::seconds;
	using std::chrono::nanoseconds;
	using std::chrono::duration_cast;
	
	const int64_t oneBillion = 1000000000;
	struct timespec ts;
	ts.tv_sec = time_t(duration_cast<seconds>(d).count());
	ts.tv_nsec = (duration_cast<nanoseconds>(d).count() % oneBillion);

	return ts;
}

//////////////////////////////////////////////////////////////////////
template <class CLK, class DUR>
std::chrono::time_point<CLK, DUR>
to_timepoint(const struct timespec & ts) noexcept
{
	DUR d = ( std::chrono::seconds(ts.tv_sec)
	        + std::chrono::nanoseconds(ts.tv_nsec));

	return std::chrono::time_point<CLK, DUR>(d);
}

//////////////////////////////////////////////////////////////////////
template <typename DURATION>
DURATION to_duration(const struct timespec & ts) noexcept
{
	DURATION d = ( std::chrono::seconds(ts.tv_sec)
	             + std::chrono::nanoseconds(ts.tv_nsec));

	return d;
}

//////////////////////////////////////////////////////////////////////
template<clock_source SRC>
typename posix_clock<SRC>::time_point posix_clock<SRC>::now()
{
	struct timespec ts;
	int rc = 0;

	if ((rc = clock_gettime(static_cast<clockid_t>(source), &ts)) != 0)
		throw make_syserr("clock_gettime failed");

	time_point ret = to_timepoint<posix_clock<SRC>,
	                              posix_clock<SRC>::duration>(ts);

	return ret;
}

//////////////////////////////////////////////////////////////////////
template <clock_source SRC>
typename posix_clock<SRC>::duration posix_clock<SRC>::resolution()
{
	struct timespec ts = { 0, 0 };
	if (clock_getres(static_cast<clockid_t>(source), &ts) != 0)
		throw make_syserr("clock_getres failed");

	return to_duration<duration>(ts);
}
#if 0
//////////////////////////////////////////////////////////////////////
inline uint64_t getTSC()
{
#if (__x86_64)
	uint32_t lo, hi;
	__asm__ __volatile__ ( "cpuid\n\t"
	                       "rdtsc"
	/* in */             : "=a" (lo), "=d" (hi)
	/* out */            : "a"(0)
	/* clobber */        : "%ebx", "%ecx", "memory");

	return (static_cast<uint64_t>(lo) | (static_cast<uint64_t>(hi) << 32));
#else
#	error "getTSC is not implemented for your architecture"
#endif
}

//////////////////////////////////////////////////////////////////////
template <class CLK>
std::tuple<Average<double>, double> getClockResolution(int samples)
{
	std::chrono::duration<double, std::nano> ns = typename CLK::duration(1);
	double resolution = ns.count();

	Average<double, unsigned long> averagePrecision;

	for (int i = 0; i < samples; ++i)
	{
		typename CLK::time_point begin = CLK::now();
		typename CLK::time_point end = CLK::now();
		assert(end >= begin);

		ns = std::chrono::duration<double, std::nano>(end - begin);
		averagePrecision(ns.count());
	}

	return std::tie(averagePrecision, resolution);
}

//////////////////////////////////////////////////////////////////////
inline double tscTicksPerSecond(size_t samples = 100,
                                struct timespec = {0, 100000}) // 100 usec
{
	Average<double, unsigned long> ticks;
	printf("Calibrating timer\n");
	struct timespec ts;

	if (clock_gettime(CLOCK_REALTIME, &ts) != 0)
		throw std::runtime_error("gettime failed");

	++ts.tv_sec;
//	ts.tv_nsec = step.tv_nsec;
	ts.tv_nsec = 0;

	if (clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &ts, nullptr) !=0)
		throw std::runtime_error("clock_nanosleep");

	volatile uint64_t begin = getTSC();
	for (size_t i = 0; i < samples; ++i)
	{
		++ts.tv_sec;
//		ts.tv_nsec += step.tv_nsec;
		
		if (clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &ts, nullptr) !=0)
			throw std::runtime_error("clock_nanosleep");

		volatile uint64_t end = getTSC();
		fprintf(stderr, "---> %" PRIu64 "\n", end - begin);

		ticks(end - begin);
		begin = end;
	}

	return ticks.mean();
}

//////////////////////////////////////////////////////////////////////
template <class F, typename... Args>
Average<double> getFunctionTiming(size_t samples, F function, Args&&... args)
{
	Average<double> avg;
	for (size_t i = 0; i < samples; ++i)
	{
		uint64_t begin = getTSC();
		function(std::forward(args)...);
		uint64_t end = getTSC();

		avg(end - begin);
	}

	return avg;
}
#endif

#endif // GUARD_TIMEUTIL_H
