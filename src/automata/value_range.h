#ifndef GUARD_VALUE_RANGE_H
#define GUARD_VALUE_RANGE_H 1

#include <cstddef>
#include <utility>

//////////////////////////////////////////////////////////////////////
template <typename T>
class value_range
{
 public:
	typedef T           value_type;
	typedef std::size_t size_type;

	value_range()
	  : begin()
	  , end()
		{ }

	value_range(const value_type & low, const value_type & high)
	  : begin(low), end(high)
		{ }

	value_range(const value_range & other)
	  : begin(other.begin)
	  , end(other.end)
		{ }

	value_range(value_range && other)
	  : begin(std::move(other.begin))
	  , end(std::move(other.end))
		{ }

	~value_range() = default;

	value_range & operator = (const value_range & other)
	{
		if (this != &other)
		{
			begin = other.begin;
			end = other.end;
		}
		return *this;
	}

	value_range & operator = (value_range && other)
	{
		std::swap(begin, other.begin);
		std::swap(end, other.end);
		return *this;
	}

	value_type min() const { return begin; }

	value_type max() const { return end; }

	size_type size() const { return (end - begin); }

	value_type midpoint() const { return ((begin + end) / 2); }

	bool contains(const value_type & v) const
		{ return ((v >= begin) && (v <= end)); }

	bool contains(const value_range & r) const
		{ return ((r.begin >= begin) && (r.end <= end)); }

	bool intersects(const value_range & other)
	{
		return (  ( other.begin >= begin && other.begin <= end )
		       || ( other.end >= begin   && other.end <= end )
		       || ( begin >= other.begin && begin <= other.end )
		       || ( end >= other.begin   && end <= other.end ) );
	}

 private:
	value_type begin, end;
};

//////////////////////////////////////////////////////////////////////
template <typename T>
bool operator < (const value_range<T> & a, const value_range<T> & b)
	{ return (a.min() < b.min()); }

#endif // GUARD_VALUE_RANGE_H
