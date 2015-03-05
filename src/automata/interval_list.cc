#include <cstdio>
#include <cstdint>
#include <set>
#include <list>
#include <vector>
#include <limits>
#include <iostream>

#include "utility/avl_tree.h"

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

	value_type & min() { return begin; }
	const value_type & min() const { return begin; }

	value_type & max() { return end; }
	const value_type & max() const { return end; }

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
template <typename T>
std::ostream & operator << (std::ostream & o, const value_range<T> & vr)
	{ return o << '[' << vr.min() << '-' << vr.max() << ']'; }

//////////////////////////////////////////////////////////////////////
template <>
std::ostream & operator << (std::ostream & o, const value_range<uint8_t> & vr)
{
	char buffer[20];
	snprintf(buffer, 20, "[%02hhx-%02hhx]", vr.min(), vr.max());
	return o << buffer;
}

//////////////////////////////////////////////////////////////////////
template <typename T>
bool operator < (const value_range<T> & a, const value_range<T> & b)
{
	if (a.min() < b.min())
		return true;
	else if (a.min() == b.min() && a.max() < b.max())
		return true;
	else
		return false;
}

//////////////////////////////////////////////////////////////////////
template <typename T>
value_range<T> find_interval(const std::vector<T> & interval_list, T val)
{
	int i = 1;
	value_range<T> retval { std::numeric_limits<T>::min(),
	                               std::numeric_limits<T>::max() };

	for (auto & x : interval_list)
	{
		++i;
		if (val <= x)
		{
			retval.max() = x;
			break;
		}

		retval.min() = x + 1;
	}
	printf("--> i = %d\n", i);
	return retval;
}

//////////////////////////////////////////////////////////////////////
template <typename T>
value_range<T> find_interval2(const std::vector<T> & list, T val)
{
	value_range<T> retval { std::numeric_limits<T>::min(),
		                    std::numeric_limits<T>::max() };
	if (list.empty())
	{
		// do nothing
	} else if (val <= list.front())
	{
		retval.max() = list.front();
	} else if (val > list.back())
	{
		retval.min() = list.back() + 1;
	} else
	{
		size_t lower = 0;
		size_t upper = (list.size() - 1);
		size_t middle;
		while (upper - lower > 1)
		{
			middle = ((upper + lower) >> 1);

			if (val <= list[middle])
				upper = middle;
			else
				lower = middle;
		}
		retval = value_range<T>{list[lower] + 1, list[upper]};
	}

	return retval;
}

//////////////////////////////////////////////////////////////////////
template <typename T>
class interval_set
{
 public:
	typedef T value_type;
	typedef value_range<value_type> interval_type;

 private:
	struct range_mapping
	{
		range_mapping(const value_type & l, const value_type & h)
		  : interval(l, h), fragment_ends() { }

		interval_type interval;
		mutable std::vector<size_t> fragment_ends;

		bool operator < (const range_mapping & other) const
			{ return (this->interval < other.interval); }
	};

	typedef std::list<typename std::set<range_mapping>::iterator> iter_list;

	std::set<range_mapping> intervals;
	std::vector<value_type> ends;
	std::vector<iter_list> reverse_mapping;
	bool dirty;

 public:

	interval_set()
	  : intervals{}
	  , ends{}
	  , reverse_mapping{}
	  , dirty(false)
		{ }

	void add_interval(const value_type & low, const value_type & high)
	{
		if (intervals.emplace(low, high).second)
			dirty = true;
	}

	void add_interval(const interval_type & i)
		{ add_interval(i.min(), i.max()); }

	void dump_ends()
	{
		if (dirty) build_ends();
		auto ei = ends.begin();
		auto rmi = reverse_mapping.begin();
		for (; ei != ends.end() && rmi != reverse_mapping.end(); ++ei, ++rmi)
		{
			std::cout << std::hex << static_cast<unsigned long>(*ei) << " { ";
			for (auto x : *rmi)
			{
				std::cout << x->interval << ' ';
			}
			std::cout << "}\n";
		}
		std::cout << std::endl;

		printf("REVERSE\n");
		for (auto y : intervals)
		{
			std::cout << y.interval << " { ";
			for (auto z : y.fragment_ends)
			{
				std::cout << std::hex << static_cast<unsigned long>(ends[z])
				          << std::dec << '(' << z << ") ";
			}
			std::cout << "}\n";
		}
	}

	struct foo
	{
		foo(const value_type & l, const value_type & h)
		  : interval(l, h), fragment_ends() { }
		interval_type interval;
		mutable std::vector<size_t> fragment_ends;

		bool operator < (const foo & other) const
			{ return (this->interval < other.interval); }
	};

 private:

	void build_ends()
	{
		std::set<value_type> s;
		for (auto & r : intervals)
		{
			r.fragment_ends.clear();
			if (r.interval.min() != std::numeric_limits<value_type>::min())
				s.emplace(r.interval.min() - 1);

			if (r.interval.max() != std::numeric_limits<value_type>::max())
				s.emplace(r.interval.max());
		}
		ends.assign(s.begin(), s.end());
		reverse_mapping.assign(ends.size(), iter_list{});

		for (auto r = intervals.begin(); r != intervals.end(); ++r)
		{
			for (size_t i = 0; i < ends.size(); ++i)
			{
				if (  (r->interval.min() <= ends[i])
				   && (r->interval.max() >= ends[i]) )
				{
					r->fragment_ends.push_back(i);
					reverse_mapping.at(i).push_back(r);
				}
			}
		}
		dirty = false;
	}
};

//////////////////////////////////////////////////////////////////////
int main()
{
	std::vector<value_range<uint32_t>> intervals;
	std::vector<uint32_t> search;
	std::vector<uint32_t> search2;
	interval_set<uint8_t> set;

#if 0
	set.add_interval(0, 9);
	set.add_interval(16, 21);
	set.add_interval(6, 17);

#endif
	set.add_interval(0x00, 0x7f);
	set.add_interval(0xc0, 0xdf);
	set.add_interval(0xe0, 0xee);
	set.add_interval(0xef, 0xef);
	set.add_interval(0xf0, 0xf7);
	set.add_interval(0xf8, 0xfb);
	set.add_interval(0xfc, 0xfd);
	set.add_interval(0x80, 0xba);
	set.add_interval(0xbb, 0xbb);
	set.add_interval(0xbc, 0xbf);
	set.add_interval(0x80, 0xbe);
	set.add_interval(0xbf, 0xbf);
	set.add_interval(0xbc, 0xbf);
	set.add_interval(0x00, 0x7f);
	set.add_interval(0xc0, 0xdf);
	set.add_interval(0xe0, 0xef);
	set.add_interval(0xf0, 0xf7);
	set.add_interval(0xf8, 0xfb);
	set.add_interval(0xfc, 0xfd);
	set.add_interval(0x00, 0x7f);
	set.add_interval(0xc0, 0xdf);
	set.add_interval(0xe0, 0xef);
	set.add_interval(0xf0, 0xf7);
	set.add_interval(0xf8, 0xfb);
	set.add_interval(0xfc, 0xfd);
	set.add_interval(0x80, 0xbf);
	set.add_interval(0x80, 0xbf);
	set.add_interval(0x80, 0xbf);
	set.add_interval(0x80, 0xbf);
	set.add_interval(0x80, 0xbf);

/*
	intervals.emplace_back(0, 9);
	intervals.emplace_back(intervals.back().max() + 1, 15);
	intervals.emplace_back(intervals.back().max() + 1, 21);
	intervals.emplace_back(intervals.back().max() + 1, 23);
	intervals.emplace_back(intervals.back().max() + 1, 24);
	intervals.emplace_back(intervals.back().max() + 1, 50);
	intervals.emplace_back(intervals.back().max() + 1, 60);
	intervals.emplace_back(intervals.back().max() + 1, 81);
	intervals.emplace_back(intervals.back().max() + 1, 85);
	intervals.emplace_back(intervals.back().max() + 1, 90);
	intervals.emplace_back(intervals.back().max() + 1,
	                       std::numeric_limits<uint32_t>::max());

	printf("SIZE = %zu\n", intervals.size());
	search.reserve(intervals.size());
	search2.reserve(intervals.size() + 1);
	//search2.push_back(0);
	for (auto & r : intervals)
	{
		search.push_back(r.max());
		search2.push_back(r.max());
		printf("%u\n", r.max());
	}

	auto r = find_interval(search, 2u);
	printf("%u -> [%u-%u]\n", 2u, r.min(), r.max());
	r = find_interval2(search2, 2u);
	printf("%u -> [%u-%u]\n", 2u, r.min(), r.max());

	r = find_interval(search, 80u);
	printf("%u -> [%u-%u]\n", 80u, r.min(), r.max());
	r = find_interval2(search2, 80u);
	printf("%u -> [%u-%u]\n", 80u, r.min(), r.max());

	r = find_interval(search, 24u);
	printf("%u -> [%u-%u]\n", 24u, r.min(), r.max());
	r = find_interval2(search2, 24u);
	printf("%u -> [%u-%u]\n", 24u, r.min(), r.max());

	r = find_interval(search, 2000u);
	printf("%u -> [%u-%u]\n", 2000u, r.min(), r.max());
	r = find_interval2(search2, 2000u);
	printf("%u -> [%u-%u]\n", 2000u, r.min(), r.max());

	r = find_interval(search, 125u);
	printf("%u -> [%u-%u]\n", 125u, r.min(), r.max());
	r = find_interval2(search2, 125u);
	printf("%u -> [%u-%u]\n", 125u, r.min(), r.max());

	r = find_interval(search, 4294967295u);
	printf("%u -> [%u-%u]\n", 4294967295u, r.min(), r.max());
	r = find_interval2(search2, 4294967295u);
	printf("%u -> [%u-%u]\n", 4294967295u, r.min(), r.max());
*/

	printf("===================\n");
	set.dump_ends();

	return 0;
}
