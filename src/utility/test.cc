#include "avl_tree.h"
#include <string>

void print()
{
	printf("hello world\n");
}

int main()
{
	avl_tree<int> tree;

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

	for (auto i = tree.rbegin(); i != tree.rend(); ++i)
		printf("\t%d (%d)\n", *i, i.base().height());

	avl_tree<std::string> empty;

	assert(empty.begin() == empty.end());

	print();
}
