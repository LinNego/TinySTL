/*第一个项目的第一个文件*/

#ifndef ALLOCATER_H
#define ALLOCATER_H
#include <cstddef>
#include "alloc.h"
#include "construct.h"
#include "type_traits.h"

namespace mystl {	
	template <typename T, typename Alloc>
	struct simple_alloc {
		static T *allocate(size_t n)
				{return n == 0 ? nullptr: (T*)Alloc::allocate(n * sizeof(T));}
		static T *allocate(void)
				{return (T*)Alloc::allocate(sizeof(T));}
		static void deallocate(T *p, size_t n)
				{Alloc::deallocate(p, sizeof(T) * n);}
		static void deallocate(T *p)
				{Alloc::deallocate(p, sizeof(T));}
	};
	
}

#endif
