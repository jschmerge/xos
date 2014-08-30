#ifndef GUARD_TIMEUTIL_H
#define GUARD_TIMEUTIL_H 1

#include <cinttypes>
#include <cstdio>
#include <chrono>
#include <tuple>
#include <cassert>
#include <system_error>

#include "average.h"

//////////////////////////////////////////////////////////////////////
template<clockid_t SOURCE>
struct PosixClock
{
	typedef std::chrono::nanoseconds duration;
	typedef duration::rep rep;
	typedef duration::period period;
	typedef std::chrono::time_point<PosixClock, duration> time_point;

	static_assert(PosixClock::duration::min() < PosixClock::duration::zero(),
	              "a clock's minimum duration cannot be less than its epoch");

	static constexpr bool is_steady = (SOURCE == CLOCK_MONOTONIC_RAW);

	static time_point now();

	static const clockid_t clockSource = SOURCE;
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
struct timespec
to_timespec(const DURATION & d) noexcept
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
from_timespec(const struct timespec & ts) noexcept
{
	DUR d = ( std::chrono::seconds(ts.tv_sec)
	        + std::chrono::nanoseconds(ts.tv_nsec));

	return std::chrono::time_point<CLK, DUR>(d);
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
template<clockid_t SRC>
typename PosixClock<SRC>::time_point PosixClock<SRC>::now()
{
	struct timespec ts;
	int rc = 0;

	if ((rc = clock_gettime(clockSource, &ts)) != 0)
		throw std::system_error(errno, std::system_category(), "clock_gettime");

	time_point ret = from_timespec<PosixClock<SRC>,
	                                      PosixClock<SRC>::duration>(ts);

	return ret;
}

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

#endif // GUARD_TIMEUTIL_H
