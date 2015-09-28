#ifndef GUARD_TABLE_H
#define GUARD_TABLE_H 1

#include <array>
#include <vector>
#include <algorithm>
#include <functional>

template <typename T, size_t N>
class lookup_table
{
 public:
	typedef std::array<size_t, N> index_type;

	static const size_t num_indexes = N;

	const index_type index_lengths;
	const size_t element_count;

	lookup_table(const std::array<size_t, N> & dims,
	             const std::initializer_list<T> & vals)
	  : index_lengths(dims)
	  , element_count(std::accumulate(dims.begin(), dims.end(), 1,
                                      std::multiplies<size_t>()))
	  , values(vals)
		{ values.resize(element_count); }

	lookup_table(const std::array<size_t, N> & dims,
	             const T & default_value = T())
	  : index_lengths(dims)
	  , element_count(std::accumulate(dims.begin(), dims.end(), 1,
                                      std::multiplies<size_t>()))
	  , values(element_count, default_value)
		{ }

	T & at(const index_type & idx)
	{
		for (size_t i = 0; i < N; ++i)
			if (idx[i] >= index_lengths[i])
				throw std::out_of_range("lookup_table::at");
		return values.at(translate_index(idx));
	}

	T at(const index_type & idx) const
	{
		for (size_t i = 0; i < N; ++i)
			if (idx[i] >= index_lengths[i])
				throw std::out_of_range("lookup_table::at");
		return values.at(translate_index(idx));
	}

	T operator [] (const index_type & idx) const
		{ return values[translate_index(idx)]; }

	T & operator [] (const index_type & idx)
		{ return values[translate_index(idx)]; }

	size_t translate_index(const index_type & idx) const
	{
		auto length_iterator = index_lengths.begin();
		return std::accumulate(
		         idx.begin(), idx.end(), 0,
		         [&length_iterator] (size_t a, size_t b) {
		             return (((*(length_iterator++)) * a) + b);
		         });
	}


	template <typename ... Args>
	T at(Args ... args)
	{
		static_assert(sizeof...(args) == N, "Wrong number of parameters");
		return values[translate_index({{args...}})];
	}

 private:
	std::vector<T> values;
};

#endif // GUARD_TABLE_H
