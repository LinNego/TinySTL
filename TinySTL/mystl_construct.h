/*SGI STL标准*/
/*内存配置，构造对象，析构对象，释放内存.*/
/*mystl_construct.h*/
/*这个头文件含有construct 和 destroy函数*/

#ifndef MYSTL_CONSTRUCT_H
#define MYSTL_CONSTRUCT_H
#include <cstddef>  //for size_t ptrdiff_t
#include <new>      //for ::operator new ::operator delete


namespace mystl {

	template <typename T>
	inline void construct(T *p, const T &value) {
		::new (p) T(value);
	}

/*如果T类型没有默认构造函数，编译将不通过*/
	template <typename T>
	inline void construct(T *p) {
		::new (p) T();
	}

	template <typename T>
	inline void destroy(T *p) {
		if(p == nullptr) return ;
		p->~T()	;
	}
}

/*
 *第一次写的水平,记录一下
 *destroy只写了一个，需要Iterator和_type_traits<T>的没写
 *STL源码剖析后续第三章才提到Iterator和_type_traits<T>
 */
/*-------------------------------------------------------------------------------*/


		

#endif
