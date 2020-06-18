//简单测试allocator
#include "TinySTL/allocator.h"
#include <vector>
using namespace std;
using namespace mystl;

int main() {
	allocator<vector<int>> alloc;
	vector<int, alloc> ve;
   	ve.push_back(1)	;
	return 0;
}
