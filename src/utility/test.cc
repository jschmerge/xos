#include "avl_tree.h"

void print()
{
	printf("hello world\n");
}

int main()
{
	avl_tree<int> tree;

	printf("Size of tree: %zu\n", sizeof(tree));

	tree.insert(1);
	tree.insert(-20);
	tree.insert(6);
	tree.insert(3);
	tree.insert(2);
	tree.insert(10);
	tree.insert(-4);

	for (const auto & i: tree)
		printf("\t%d\n", i);

	printf("reverse:\n");

	for (auto i = tree.rbegin(); i != tree.rend(); ++i)
		printf("\t%d\n", *i);

	avl_tree<int> empty;

	assert(empty.begin() == empty.end());

	print();
}
