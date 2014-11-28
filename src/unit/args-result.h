#ifndef GUARD_ARGS_AND_RESULT_H
#define GUARD_ARGS_AND_RESULT_H 1

#include <utility>
#include <tuple>

#include "utility/index_sequence.h"

template <typename RESULT, typename ...Args>
struct args_and_result
{
	args_and_result(const Args&... args, const RESULT & res)
	  : operands(args...), result(res)
		{ }

	template <typename F>
	RESULT apply(F func)
	{
		return apply_function(func, operands);
	}

	std::tuple<const Args...> operands;
	const RESULT result;
};

#endif // GUARD_ARGS_AND_RESULT_H
