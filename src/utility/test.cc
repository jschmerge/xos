#include "avl_tree.h"
#include <string>

#define my_assert(expr) do { \
	if ( ! (expr)) { \
		fputs("Assertion failed: '" #expr "'\n", stderr); \
		abort(); \
	} \
} while(0)

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

int main()
{
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
		printf("\t%d, (%d, %d)\n", *i, i.height(), i.balance());
		tree.dump();
	}
	for (int x = 0; x > -33; --x)
	{
		tree.insert(x);
		printf("----\n");
		tree.dump();
	}
#if 0
#endif

	printf("-----------------\n");
	for (auto i = tree.begin(); i != tree.end(); ++i)
		printf("\t%d, (%d, %d)\n", *i, i.height(), i.balance());

	printf("reverse:\n");

	for (auto j = tree.rbegin(); j != tree.rend(); ++j)
	{
		auto i = j.base(); --i;
		printf("\t%d, (%d, %d)\n", *j, i.height(), i.balance());
	}

	avl_tree<std::string> empty;

	my_assert(empty.begin() == empty.end());
	tree.dump();

	avl_tree<int> mycopy(tree);
	for (auto i = mycopy.begin(); i != mycopy.end(); ++i)
		printf("\t%d, (%d, %d)\n", *i, i.height(), i.balance());

	printf("-----------------\n");
	mycopy.dump();
}
