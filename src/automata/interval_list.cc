#include <cstdio>
#include <cstdint>
#include <vector>
#include <limits>

//////////////////////////////////////////////////////////////////////
// Represents the range [low,high]
template <typename VAL_T>
class value_range
{
 public:
	typedef VAL_T value_type;
	typedef std::size_t size_type;

	value_range() : begin(0) , end(0) { }

	value_range(const value_type & low, const value_type & high)
	  : begin(low) , end(high) { }

/*
	value_range(const value_range & other) = default;
	value_range(value_range && other) = default;
	~value_range() = default;
*/

	value_type min() const { return begin; }

	value_type max() const { return end; }

	size_type size() const { return (end - begin); }

	bool intersects(const value_range & other)
	{
		return (  ( other.begin >= begin && other.begin <= end )
		       || ( other.end >= begin   && other.end <= end )
		       || ( begin >= other.begin && begin <= other.end )
		       || ( end >= other.begin   && end <= other.end ) );
	}

	bool operator == (const value_range & other)
		{ return ( (begin == other.begin) && (end == other.end) ); }

 private:
	value_type begin, end;
};

//////////////////////////////////////////////////////////////////////
value_range<uint32_t>
find_interval(std::vector<uint32_t> & interval_list, uint32_t val)
{
	value_range<uint32_t> retval;
	uint32_t last = 0;
	for (auto & x : interval_list)
	{
		if (val <= x)
		{
			retval = value_range<uint32_t>(last, x);
			break;
		}

		last = x + 1;
	}
	return retval;
}

//////////////////////////////////////////////////////////////////////
value_range<uint32_t>
find_interval2(std::vector<uint32_t> & interval_list, uint32_t val)
{
	value_range<uint32_t> retval;
	size_t index = (interval_list.size() >> 1);
	size_t lbound = 0;
	size_t ubound = interval_list.size() - 1;

	while ((ubound - lbound) > 1)
	{
		printf("index = %zu, l = %zu, u = %zu\n", index, lbound, ubound);
		if (val <= interval_list[index])
		{
			ubound = index;
		} else if (val > interval_list[index])
		{
			lbound = index;
		}

		index = ((ubound + lbound) >> 1);
	}

	printf("index = %zu, l = %zu, u = %zu\n", index, lbound, ubound);
	printf("----> %u (%u-%u)\n", interval_list[index],
	       interval_list[lbound], interval_list[ubound]);

	return retval;
}

//////////////////////////////////////////////////////////////////////
int main()
{
	std::vector<value_range<uint32_t>> intervals;
	std::vector<uint32_t> search;

	intervals.emplace_back(0, 20);
	intervals.emplace_back(intervals.back().max() + 1, 23);
	intervals.emplace_back(intervals.back().max() + 1, 24);
	intervals.emplace_back(intervals.back().max() + 1, 50);
	intervals.emplace_back(intervals.back().max() + 1, 60);
	intervals.emplace_back(intervals.back().max() + 1, 81);
	intervals.emplace_back(intervals.back().max() + 1, 85);
	intervals.emplace_back(intervals.back().max() + 1, 90);
	intervals.emplace_back(intervals.back().max() + 1, 99);
	intervals.emplace_back(intervals.back().max() + 1,
	                       std::numeric_limits<uint32_t>::max());

	search.reserve(intervals.size());
	for (auto & r : intervals)
	{
		search.push_back(r.max());
		printf("%u\n", r.max());
	}

	auto r = find_interval(search, 2);
	printf("%u -> [%u-%u]\n", 2, r.min(), r.max());
	r = find_interval2(search, 2);

	r = find_interval(search, 80);
	printf("%u -> [%u-%u]\n", 80, r.min(), r.max());
	r = find_interval2(search, 80);

	r = find_interval(search, 24);
	printf("%u -> [%u-%u]\n", 24, r.min(), r.max());
	r = find_interval2(search, 24);

	r = find_interval2(search, 2000);

	return 0;
}
