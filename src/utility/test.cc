#include "avl_tree.h"
#include <unistd.h>
#include <locale>
#include <string>
#include <set>
#include <random>
#include "time/timeutil.h"

#include "bulk_allocator.h"

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

	printf("insertion took %.9f seconds for %ld random new values\n",
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

	printf("insertion took %.9f seconds for %ld-%ld ordered values\n",
	       d.count(), starting_size, container.size());
	fflush(stdout);
}

//////////////////////////////////////////////////////////////////////
int main()
{
	std::locale::global(std::locale("en_GB.UTF-8"));

	avl_tree<int> tree;
	//avl_tree<int, std::greater<int>> tree;

	printf("Size of tree: %zu\n", sizeof(tree));

#if 0
	test_insert(tree, 1);
	test_insert(tree, -4);
	test_insert(tree, -20);
	test_insert(tree, 6);
	test_insert(tree, 3);
	test_insert(tree, 2);
	tree.emplace(10);
	test_insert_dup(tree, 10);
	test_insert(tree, 11);
	test_insert(tree, 12);
	test_insert(tree, 13);
	test_insert(tree, 14);
	test_insert(tree, 15);
	test_insert(tree, 16);
	test_insert(tree, 17);
	test_insert(tree, -21);
#endif

	for (int x = 0; x < 15; ++x)
	{
		tree.insert(x);
		printf("----\n");
		for (auto i = tree.begin(); i != tree.end(); ++i)
			printf("\t%d, (%zd, %d)\n", *i, i.height(), i.balance());
		tree.dump();
	}

	for (int x = 0; x > -17; --x)
	{
		tree.insert(x);
		printf("----\n");
		tree.dump();
	}

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

	printf("-----------------\n");
	while (mycopy.size())
	{
		printf("===> Deleting %d\n", *mycopy.begin());
		mycopy.erase(mycopy.begin());
	}

	my_assert(mycopy.empty());

	const int64_t max_values = 1000000;
#if 0
#endif
	for (int64_t i = 1; i <= max_values; i *= 10)
	{
		printf("i = %'ld\n------------------------------------\n", i);
		for (int j = 0; j < 3; ++j)
		{
			std::mt19937_64 engine(0);
			size_t min =  ~0, max = 0;
			{
				homogenous_arena<avl_tree_node<int64_t>> arena{i * 10ul};
				bulk_allocator<avl_tree_node<int64_t>> alloc{&arena};
				avl_tree<int64_t, std::less<int64_t>,
			         bulk_allocator<avl_tree_node<int64_t>>> a{alloc};
				printf("AVL:\n");
				test_random_insert(a, engine, i);
				test_random_insert(a, engine, i);
				test_random_insert(a, engine, i);

				for (auto x = a.begin(); x != a.end(); ++x)
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

			engine.seed(0);
			printf("RedBlack:\n");
			{
				homogenous_arena<std::_Rb_tree_node<int64_t>> arena{i * 10ul};
				bulk_allocator<std::_Rb_tree_node<int64_t>> alloc{&arena};
				std::set<int64_t, std::less<int64_t>,
			             bulk_allocator<int64_t>> b{std::less<int64_t>{},alloc};
				test_random_insert(b, engine, i);
				test_random_insert(b, engine, i);
				test_random_insert(b, engine, i);
			}
		}
	}

	for (int64_t i = 1; i <= (max_values * 10); i *= 10)
	{

		printf("i = %'ld\n--------------------------------------\n", i);
		for (int j = 0; j < 3; ++j)
		{
			{
				homogenous_arena<avl_tree_node<int64_t>>
				                         arena{std::min(i * 10ul, 500000000ul)};
				bulk_allocator<avl_tree_node<int64_t>> alloc{&arena};
			
				size_t min =  ~0, max = 0;
				avl_tree<int64_t, std::less<int64_t>,
			         	bulk_allocator<avl_tree_node<int64_t>>> a{alloc};
				printf("AVL:\n");
				for (int k = 0; k < 10 && a.size() < 500000000; ++k)
					test_ordered_insert(a, i);

				for (auto x = a.begin(); x != a.end(); ++x)
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

			{
				homogenous_arena<std::_Rb_tree_node<int64_t>>
				                         arena{std::min(i * 10ul, 500000000ul)};
				bulk_allocator<std::_Rb_tree_node<int64_t>> alloc{&arena};
			

				printf("RB:\n");
				std::set<int64_t, std::less<int64_t>,
			         	bulk_allocator<int64_t>> b{std::less<int64_t>{}, alloc};
				for (int k = 0; k < 10 && b.size() < 500000000; ++k)
					test_ordered_insert(b, i);
			}
		}
	}
#if 0
#endif
}
