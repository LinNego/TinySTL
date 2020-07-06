/*SGI STL的alloc*/
/*memory pool*/
/*SGI的两级内存分配*/
/*主要模仿SCI的二级分配并加上自己的东西*/
/*先自己尝试根据STL源码剖析解释的那样写一个第一个级的分配器*/
#ifndef MYSTL_ALLOC_H
#define MYSTL_ALLOC_H

#include <cstdlib>  //for malloc() , free()
#include <cstddef>
#include <exception> //for throw
#include <new>

/*主要点在内存分配不足的时候需要自己设定handler函数*/

namespace mystl {

	template <int inst>
	class alloc_prime {
		typedef void (*hander)(); //为了简化声明
		alloc_prime(const alloc_prime&)	= delete;
		alloc_prime& operator=(const alloc_prime&) = delete;
	private:
		/*都为static的原因*/
		/*我认为有一个就是可以不用构造用户不需要构造一个对象可以直接使用.2020.6.19*/
		static void* oom_malloc(size_t);  
		static void* oom_realloc(void *, size_t);
		/*这个设为静态成员的目的？*/
		static hander oom_hander;
		void* realloc(void *p, size_t new_sz);
			
	public:
  		static void* allocate(size_t n) {
			void *result = malloc(n);
			if(result == nullptr) result = oom_malloc(n);
			return result;
		}

		static void deallocate(void *p) {
			free((char*)p);
		}

		static void deallocate(void *p, size_t n) {
			for(int i = 0; i < n; ++i) {
				free((char*)(p + i));
			}
		}

		static void* reallocate(void *p, size_t, size_t new_size) {
			void *result = realloc(p, new_size);
			if(result == nullptr) result = oom_realloc(p, new_size);
			return result;
		}

		hander set_myalloc_hander(hander new_hander) {
			hander old_hander = oom_hander;
			oom_hander = new_hander;
			return old_hander;	
		}
	};
	template <int inst>
	typename alloc_prime<inst>::hander alloc_prime<inst>::oom_hander = nullptr;

	/*
	template <int inst>
	void* alloc_prime<inst>::realloc(void *p, size_t new_sz) {
		void *p;
		return p;	
	}
	*/

	template <int inst>
	void* alloc_prime<inst>::oom_malloc(size_t sz) {
		void * result;
		hander cur_hander;
		for(; ;) {
			cur_hander = oom_hander;
			if(cur_hander == nullptr) {
				throw std::bad_alloc();
			}
			(*cur_hander)();
			result = malloc(sz);
			if(result) return result;

		}
	}

	template <int inst>
	void* alloc_prime<inst>::oom_realloc(void *p, size_t n) {
		void *result;
		hander cur_hander;
		for(; ;) {
			cur_hander = oom_hander;
			if(cur_hander == nullptr) {
				throw std::bad_alloc();
			}
			(*cur_hander)();
			result = realloc(p, n);
			if(result) return result;
			(*cur_hander)();
		}
	}
	/*realloc where ?*/
	using malloc_alloc = alloc_prime<0>;

	
} 
/*以上用到了极少C++11的特性*/
/*----------------------------------------------------------------------------------分割线2020.6.19*/

#endif
