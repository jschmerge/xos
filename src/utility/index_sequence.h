#ifndef GUARD_INDEX_SEQUENCE_H
#define GUARD_INDEX_SEQUENCE_H 1

#include <cstdint>
#include <tuple>

/// index_sequence is useful for variadic expansion of typle indices
template <std::size_t... SEQ>
struct index_sequence
	{ static constexpr std::size_t size() { return sizeof...(SEQ); } };

/// index_sequence_generator creates an index_sequence and maps it to
/// index_sequence_generator<N>::type
template <std::size_t N, std::size_t ... SEQ>
struct index_sequence_generator
  : index_sequence_generator<N - 1, N - 1, SEQ...> { };

/// index_sequence_generator specialization for terminal recursive case
template <std::size_t ... SEQ>
struct index_sequence_generator<0, SEQ...>
	{ typedef index_sequence<SEQ...> type; };

/// helper for apply_function
template <typename Func, typename ... Args, std::size_t ... IDX>
auto dispatch_function(Func f, std::tuple<Args...> & args,
                       const index_sequence<IDX...> &)
	-> typename std::result_of<Func(Args &...)>::type
{
	return f(std::get<IDX>(args)...);
}

/// apply_function calls a function with the arguments supplied as a tuple
template <typename Func, typename ...Args>
auto apply_function(Func f, std::tuple<Args...> & args)
	-> typename std::result_of<Func(Args&...)>::type
{
	typename index_sequence_generator<sizeof...(Args)>::type indexes;
	return dispatch_function(f, args, indexes);
}

#endif // GUARD_INDEX_SEQUENCE_H
