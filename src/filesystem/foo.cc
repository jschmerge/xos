#include <type_traits>
#include <array>
#include <vector>
#include <iterator>

template <typename T, typename Enable = void>
struct is_iterable : std::false_type { };

template <typename T>
struct is_iterable <
	T,	typename std::enable_if<
			std::is_base_of<
				std::forward_iterator_tag, typename std::iterator_traits<
					typename std::decay<T>::type
			>::iterator_category >::value >::type>
{
	static constexpr bool value = true;
};
                                                       
static_assert( ! is_iterable<int        >::value, "");
static_assert( ! is_iterable<int &      >::value, "");
static_assert( ! is_iterable<int &&     >::value, "");

static_assert(   is_iterable<int *      >::value, "");
static_assert(   is_iterable<int **     >::value, "");
static_assert(   is_iterable<int []     >::value, "");
static_assert(   is_iterable<int [][10] >::value, "");

static_assert(   is_iterable<std::vector<int>::iterator>::value, "");
static_assert(   is_iterable<std::array<int, 0>::iterator>::value, "");
static_assert(   is_iterable<std::array<int, 10>::iterator>::value, "");
