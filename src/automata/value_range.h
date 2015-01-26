#ifndef GUARD_VALUE_RANGE_H
#define GUARD_VALUE_RANGE_H 1

#include <cstddef>
#include <utility>

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

	bool contains(const value_type & v) const
		{ return ((v >= begin) && (v <= end)); }

	value_type min() const
		{ return begin; }

	value_type max() const
		{ return end; }

	size_type size() const
		{ return (end - begin); }

	value_type midpoint() const
		{ return ((begin + end) / 2); }

	bool less_than_by_position(const value_range & other)
	{
		bool rc = false;
		if (this->midpoint() < other.midpoint())
			rc = true;
		else if (this->midpoint() == other.midpoint())
			rc = (this->size() < other.size());

		return rc;
	}

	bool less_than_by_size(const value_range & other)
	{
		bool rc = false;
		if (this->size() < other.size())
			rc = true;
		else if (this->size() == other.size())
			rc = (this->midpoint() < other.midpoint());

		return rc;
	}
	

 private:
	value_type begin, end;
};

#endif // GUARD_VALUE_RANGE_H
