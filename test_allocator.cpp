/*简单测试allocator.h*/
#include "TinySTL/allocator.h"
#include <vector>
#include <cstdio>


int main() {
  	mystl::allocator<int> alloc;
	std::vector<int, alloc> ve;
	int *p = alloc.allocate(1);
	alloc.construct(p, 5);
	printf("%d\n", *p);
	return 0;
}
