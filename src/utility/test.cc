#include "avl_tree.h"
#include <malloc.h>
#include <unistd.h>
#include <locale>
#include <string>
#include <set>
#include <random>
#include "time/timeutil.h"

#include "bulk_allocator.h"

#define USE_BULK_ALLOC 1

#define my_assert(expr) do { \
	if ( ! (expr)) { \
		fputs("Assertion failed: '" #expr "'\n", stderr); \
		abort(); \
	} \
} while(0)

//////////////////////////////////////////////////////////////////////
void test_insert(avl_tree<int> & tree, int val)
{
	printf("ADDING %d\n", val);
	auto p = tree.insert(val);
	my_assert(p.second == true && *p.first == val);
}

//////////////////////////////////////////////////////////////////////
void test_insert_dup(avl_tree<int> & tree, int val)
{
	auto p = tree.insert(val);
	my_assert(p.second == false && *p.first == val);
}

//////////////////////////////////////////////////////////////////////
template<typename T>
void test_random_insert(T & container,
                        std::mt19937_64 & engine,
                        int total)
{
	int64_t seed = container.empty() ? 0 : container.size();
	std::uniform_int_distribution<int64_t> dist;

	auto begin = posix_clock<clock_source::realtime>::now();

	for (int i = 0; i < total; ++i)
	{
		int64_t value = dist(engine);
		container.insert(value);
	}

	auto end = posix_clock<clock_source::realtime>::now();

	std::chrono::duration<double> d = end - begin;

	printf("insertion took %'.9f seconds for %'ld random new values\n",
	       d.count(), container.size() - seed);
	fflush(stdout);
}

//////////////////////////////////////////////////////////////////////
template<typename T>
void test_ordered_insert(T & container, int64_t total = 10000000)
{
	int64_t starting_size = container.size();
	auto begin = posix_clock<clock_source::realtime>::now();

	for (int64_t i = container.size(); i < (starting_size + total); ++i)
	{
		container.insert(container.end(), i);
	}

	auto end = posix_clock<clock_source::realtime>::now();

	std::chrono::duration<double> d = end - begin;

	printf("insertion took %'.9f seconds for %'ld-%'ld ordered values\n",
	       d.count(), starting_size, container.size());
	fflush(stdout);
}

//////////////////////////////////////////////////////////////////////
template <typename T, typename C, typename A>
void print_leaf_heights(const avl_tree<T,C,A> & t)
{
	size_t min =  ~0, max = 0;
	for (auto x = t.begin(); x != t.end(); ++x)
	{
		if (x.is_leaf_node())
		{
			size_t h = x.height();
			if (h < min)
				min = h;
			if (h > max)
				max = h;
		}
	}
	printf("Height min = %zu, max = %zu\n", min, max);
}

//////////////////////////////////////////////////////////////////////
template <typename T>
void time_ordered_delete(T & container)
{
	size_t n = container.size();
	auto begin = posix_clock<clock_source::realtime>::now();
	for (auto i = container.begin(); i
	     != container.end();
	     i = container.erase(i)) { }
	auto end = posix_clock<clock_source::realtime>::now();

	std::chrono::duration<double> d = end - begin;
	printf("\nDeletion took %'.9f seconds for %'zd values\n\n", d.count(), n);
}

//////////////////////////////////////////////////////////////////////
void test_performance()
{
	const int64_t min_values = 1000;
	const int64_t max_values = 10000000;
	const size_t  limit      = 50000000;
#if 0
	for (int64_t i = min_values; i <= max_values; i *= 10)
	{
		printf("i = %'ld\n------------------------------------\n", i);
		for (int j = 0; j < 3; ++j)
		{
			std::mt19937_64 engine(0);
			{
#if USE_BULK_ALLOC
				homogenous_arena<avl_tree_node<int64_t>>
				  arena{std::min((i * 10ul), limit)};

				bulk_allocator<avl_tree_node<int64_t>> alloc{&arena};
				avl_tree<int64_t, std::less<int64_t>,
			         bulk_allocator<avl_tree_node<int64_t>>> a{alloc};
#else
				avl_tree<int64_t> a;
#endif
				printf("AVL:\n");
				for (int k = 0; k < 10 && a.size() < limit; ++k)
					test_random_insert(a, engine, i);
				print_leaf_heights(a);
				time_ordered_delete(a);
			}

			engine.seed(0);
			printf("RedBlack:\n");
			{
#if USE_BULK_ALLOC
				homogenous_arena<std::_Rb_tree_node<int64_t>>
				  arena{std::min((i * 10ul), limit)};

				bulk_allocator<std::_Rb_tree_node<int64_t>> alloc{&arena};
				std::set<int64_t, std::less<int64_t>,
			             bulk_allocator<int64_t>> b{std::less<int64_t>{},alloc};
#else
				std::set<int64_t> b;
#endif
				for (int k = 0; k < 10 && b.size() < limit; ++k)
					test_random_insert(b, engine, i);
				time_ordered_delete(b);
			}
		}
	}
#endif

	for (int64_t i = min_values; i <= max_values; i *= 10)
	{

		printf("i = %'ld\n--------------------------------------\n", i);
		for (int j = 0; j < 3; ++j)
		{
			{
#if USE_BULK_ALLOC
				homogenous_arena<avl_tree_node<int64_t>>
				  arena{std::min((i * 10ul), limit)};

				bulk_allocator<avl_tree_node<int64_t>> alloc{&arena};

				avl_tree<int64_t, std::less<int64_t>,
				      bulk_allocator<avl_tree_node<int64_t>>> a{alloc};
#else
				avl_tree<int64_t> a;
#endif
				printf("AVL:\n");
				for (int k = 0; k < 10 && a.size() < limit; ++k)
					test_ordered_insert(a, i);

//				print_leaf_heights(a);
				time_ordered_delete(a);
			}

			{
#if USE_BULK_ALLOC
				homogenous_arena<std::_Rb_tree_node<int64_t>>
				  arena{std::min((i * 10ul), limit)};

				bulk_allocator<std::_Rb_tree_node<int64_t>> alloc{&arena};

				std::set<int64_t, std::less<int64_t>,
				         bulk_allocator<int64_t>> b{std::less<int64_t>{},
				                                    alloc};
#else
				std::set<int64_t> b;
#endif
				printf("RB:\n");
				for (int k = 0; k < 10 && b.size() < limit; ++k)
					test_ordered_insert(b, i);

				time_ordered_delete(b);
			}
		}
	}

#if USE_BULK_ALLOC
	{
		homogenous_arena<avl_tree_node<int64_t>> arena{limit};
		bulk_allocator<avl_tree_node<int64_t>> alloc{&arena};
		avl_tree<int64_t, std::less<int64_t>,
		         bulk_allocator<avl_tree_node<int64_t>>> a{alloc};
#else
		avl_tree<int64_t> a;
#endif
		auto begin = posix_clock<clock_source::realtime>::now();
		for (uint64_t i = 0; i < limit; ++i)
			a.insert(a.end(), i);
		auto end = posix_clock<clock_source::realtime>::now();
		std::chrono::duration<double> d = end - begin;
		printf("avl::insert() took %'.9f seconds for %'lu values\n",
		       d.count(), limit);
		begin = posix_clock<clock_source::realtime>::now();
		a.clear();
		end = posix_clock<clock_source::realtime>::now();
		d = end - begin;
		printf("avl::clear() took %'.9f seconds for %'lu values\n",
		       d.count(), limit);
	}


	{
#if USE_BULK_ALLOC
		homogenous_arena<std::_Rb_tree_node<int64_t>> arena{limit};
		bulk_allocator<std::_Rb_tree_node<int64_t>> alloc{&arena};

		std::set<int64_t, std::less<int64_t>,
		         bulk_allocator<int64_t>> b{std::less<int64_t>{}, alloc};
#else
		std::set<int64_t> b;
#endif
		auto begin = posix_clock<clock_source::realtime>::now();
		for (uint64_t i = 0; i < limit; ++i)
			b.insert(b.end(), i);
		auto end = posix_clock<clock_source::realtime>::now();
		std::chrono::duration<double> d = end - begin;
		printf("rb::insert() took %'.9f seconds for %'lu values\n",
		       d.count(), limit);

		begin = posix_clock<clock_source::realtime>::now();
		b.clear();
		end = posix_clock<clock_source::realtime>::now();
		d = end - begin;
		printf("rb::clear() took %'.9f seconds for %'lu values\n",
		       d.count(), limit);
	}
}
//////////////////////////////////////////////////////////////////////
int main()
{
	std::locale::global(std::locale("en_GB.UTF-8"));

	avl_tree<int> tree;

	printf("Size of tree: %zu\n", sizeof(tree));

	for (int x = -1; x > -16; --x)
	{
		tree.insert(x);
		printf("----\n");
		tree.dump();
	}

	for (int x = 1; x < 16; ++x)
	{
		tree.insert(x);
		printf("----\n");
		tree.dump();
	}

	my_assert(tree.find(1) != tree.end());

	printf("-----------------\n");
	for (auto i = tree.begin(); i != tree.end(); ++i)
		printf("\t%d, (%zd, %d)\n", *i, i.height(), i.balance());

	printf("reverse:\n");

	for (auto j = tree.rbegin(); j != tree.rend(); ++j)
	{
		auto i = j.base(); --i;
		printf("\t%d, (%zd, %d)\n", *j, i.height(), i.balance());
	}

	avl_tree<std::string> empty;

	my_assert(empty.begin() == empty.end());
	tree.dump();

	avl_tree<int> mycopy(tree);
	for (auto i = mycopy.begin(); i != mycopy.end(); ++i)
		printf("\t%d, (%zd, %d)\n", *i, i.height(), i.balance());
	mycopy.dump();

	auto q = mycopy.lower_bound(0);
	printf("---> tree.lower_bound(0) = %d\n", *q);

	printf("-----------------\n");
	while (mycopy.size())
	{
		printf("===> Deleting %d\n", *mycopy.begin());
		mycopy.erase(mycopy.begin());
	}

	my_assert(mycopy.empty());

	test_performance();

	printf("-------------------------\n");
	printf("-------------------------\n");

	avl_tree<int> xyz{2, 5, 7};
	my_assert(*xyz.lower_bound(1) == 2);
	my_assert(*xyz.lower_bound(4) == 5);
	my_assert(*xyz.lower_bound(6) == 7);
	my_assert(xyz.lower_bound(8) == xyz.end());

	printf("--->%d\n", *xyz.upper_bound(6));

	my_assert(*xyz.upper_bound(6) == 7);

}
