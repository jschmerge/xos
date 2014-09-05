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


/// Get the return type for a function
template <class F> struct return_type;
template <class R, class ...A> struct return_type<R(*)(A...)>
	{ typedef R type; };

/// helper for apply_function
template <typename F, typename TUPLE, std::size_t ... INDEXES>
auto dispatch_function(F f, TUPLE args, const index_sequence<INDEXES...>)
    -> typename return_type<F>::type
{
	return f(std::get<INDEXES>(args)...);
}

/// apply_function calls a function with the arguments supplied as a tuple
template <typename R, typename ... F_Args, typename ... T_Args>
R apply_function(R f(F_Args...), std::tuple<T_Args...> args)
{
	static_assert(sizeof...(F_Args) == sizeof...(T_Args),
	              "Function takes a differing number of arguments "
	              "than are supplied in tuple argument");
	typename index_sequence_generator<sizeof...(T_Args)>::type indexes;

	R result = dispatch_function(f, args, indexes);
	return result;
}


#endif // GUARD_INDEX_SEQUENCE_H
