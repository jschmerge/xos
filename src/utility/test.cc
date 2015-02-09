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
	test_insert(tree, -20);
	test_insert(tree, -4);
	test_insert(tree, 6);
	test_insert(tree, 3);
	test_insert(tree, 2);
	tree.emplace(10);
	test_insert_dup(tree, 10);

	for (auto i = tree.begin(); i != tree.end(); ++i)
		printf("\t%d, (%d)\n", *i, i.height());

	printf("reverse:\n");

	for (auto j = tree.rbegin(); j != tree.rend(); ++j)
	{
		auto i = j.base(); --i;
		printf("\t%d (%d)\n", *j, i.height());
	}

	avl_tree<std::string> empty;

	my_assert(empty.begin() == empty.end());

	avl_tree<int> mycopy(tree);
	for (auto x : mycopy)
		printf("-> %d\n", x);

	tree.dump();
}
