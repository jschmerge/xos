#include "avl_tree.h"
#include <string>

void print()
{
	printf("hello world\n");
}

struct foo {
	int y;
	std::less<int> x;
};

int main()
{
	avl_tree<int> tree;
	//avl_tree<int, std::greater<int>> tree;

	printf("Size of tree: %zu\n", sizeof(tree));

	tree.insert(-20);
	tree.insert(-4);
	tree.insert(1);
	tree.insert(6);
	tree.insert(3);
	tree.insert(2);
	tree.emplace(10);

	for (auto i = tree.begin(); i != tree.end(); ++i)
		printf("\t%d, (%d)\n", *i, i.height());

	printf("reverse:\n");

	for (auto j = tree.rbegin(); j != tree.rend(); ++j)
	{
		auto i = j.base(); --i;
		printf("\t%d (%d)\n", *j, i.height());
	}

	avl_tree<std::string> empty;

	assert(empty.begin() == empty.end());

	avl_tree<int> mycopy(tree);
	for (auto x : mycopy)
		printf("-> %d\n", x);

	print();
}
