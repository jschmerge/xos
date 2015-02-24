#include "avl_tree.h"
#include <unistd.h>
#include <locale>
#include <string>
#include <set>
#include <random>
#include "time/timeutil.h"

#define my_assert(expr) do { \
	if ( ! (expr)) { \
		fputs("Assertion failed: '" #expr "'\n", stderr); \
		abort(); \
	} \
} while(0)

const int64_t max_values = 10000000;

void test_insert(avl_tree<int> & tree, int val)
{
	printf("ADDING %d\n", val);
	auto p = tree.insert(val);
	my_assert(p.second == true && *p.first == val);
}

void test_insert_dup(avl_tree<int> & tree, int val)
{
	auto p = tree.insert(val);
	my_assert(p.second == false && *p.first == val);
}

template<typename T>
void test_speed(T & container, int total = 10000000)
{
	int64_t seed = container.empty() ? 0 : container.size();
	std::default_random_engine engine(seed);
	std::uniform_int_distribution<int64_t> dist(-(total * 10), total * 10);

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

template<typename T>
void test_ordered_insert(T & container, int64_t total = 10000000)
{
	auto begin = posix_clock<clock_source::realtime>::now();

	for (int64_t i = 0; i < total; ++i)
	{
		container.insert(container.end(), i);
		//container.insert(i);
	}

	auto end = posix_clock<clock_source::realtime>::now();

	std::chrono::duration<double> d = end - begin;

	printf("insertion took %.9f seconds for %ld ordered values\n",
	       d.count(), container.size());
	fflush(stdout);
}

int main()
{
	std::locale::global(std::locale("en_GB.UTF-8"));

	avl_tree<int> tree;
	//avl_tree<int, std::greater<int>> tree;

	printf("Size of tree: %zu\n", sizeof(tree));

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

	for (int x = 0; x < 31; ++x)
	{
		tree.insert(x);
		printf("----\n");
		for (auto i = tree.begin(); i != tree.end(); ++i)
			printf("\t%d, (%zd, %d)\n", *i, i.height(), i.balance());
		tree.dump();
	}

	for (int x = 0; x > -33; --x)
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
	mycopy.dump();


#if 0
	for (int64_t i = 1; i <= max_values; i *= 10)
	{
		avl_tree<int64_t> a;
		std::set<int64_t> b;
		printf("i = %'ld\n------------------------------------\n", i);
		for (int j = 0; j < 3; ++j)
		{
			size_t min =  ~0, max = 0;
			printf("AVL:\n");
			test_speed(a, i);
			test_speed(a, i);
			test_speed(a, i);

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
			a.clear();

			printf("RedBlack:\n");
			test_speed(b, i);
			test_speed(b, i);
			test_speed(b, i);
			b.clear();
		}
	}
#endif

	for (int64_t i = 1; i <= max_values; i *= 10)
	{
		printf("i = %'ld\n--------------------------------------\n", i);
		for (int j = 0; j < 3; ++j)
		{
			size_t min =  ~0, max = 0;
			avl_tree<int64_t> a;
			std::set<int64_t> b;
			printf("AVL:\n");
			test_ordered_insert(a, i);
			test_ordered_insert(a, i);
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
			a.clear();

			printf("RB:\n");
			test_ordered_insert(b, i);
			test_ordered_insert(b, i);
			test_ordered_insert(b, i);
			b.clear();
		}
	}
}
