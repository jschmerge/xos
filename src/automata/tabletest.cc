#include <cstdio>

#include "table.h"

int main()
{
	lookup_table<int, 2> tab({{3, 3}}, { 0, 1, 2, 3, 4, 5, 6, 7, 8 });

	for (auto a : std::initializer_list<std::array<size_t, 2>>{
	                                   { {0, 0} }, { {0, 1} }, { {0, 2} },
	                                   { {1, 0} }, { {1, 1} }, { {1, 2} },
	                                   { {2, 0} }, { {2, 1} }, { {2, 2} } } )
	{
		printf("--> %zu -> %d\n", tab.translate_index(a), tab.at(a));
	}
}
