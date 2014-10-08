#include <type_traits>
#include <array>
#include <vector>
#include <iterator>

// Put these in an unamed namespace since they conflict with c++14 features
namespace {

template <bool B, typename T = void> 
using enable_if_t = typename std::enable_if<B, T>::type;

template <class T> 
using decay_t = typename std::decay<T>::type;

template <class T> 
using iterator_category_t =
	typename std::iterator_traits<decay_t<T>>::iterator_category;

template<class T, class U>
constexpr bool is_base_of()
	{ return std::is_base_of<T, U>::value; }

} // namespace

template <typename T, typename Enable = void>
struct is_iterable : std::false_type { };

template <typename T>
struct is_iterable <T, enable_if_t<is_base_of<std::input_iterator_tag,
                                              iterator_category_t<T>>()>>
  : std::true_type { };
                                                       
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
static_assert(   is_iterable<std::initializer_list<int>::iterator>::value, "");
auto x = { 1, 2, 3 };
static_assert(   is_iterable<decltype(x)::iterator>::value, "");

int main()
{
}
