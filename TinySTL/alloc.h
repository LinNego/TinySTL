/*LinNego*/

#ifndef MYSTL_ALLOC_H
#define MYSTL_ALLOC_H
#include <cstddef>
#include <cstring>
#include <cstdlib>
#include <exception>
#include <new>

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

		static void deallocate(void *p, size_t) {
			free(static_cast<char*>(p));
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

namespace mystl {
	enum {ALIGN = 8};
	enum {MAX_BYTES = 128};
	enum {NFREELISTS = MAX_BYTES / ALIGN};

	template <bool threads, int inst>
	class alloc_optimize {	
		/*私以为改成static成员并且放进类里面可以隐藏数据*/
		alloc_optimize(const alloc_optimize&) = delete;
		alloc_optimize& operator=(const alloc_optimize&) = delete;
	public:
		static void* allocate(size_t n);
		static void deallocate(void *p, size_t);
		static void* reallocate(void *p, size_t old_sz, size_t new_sz);

	private:
		static size_t round_up(size_t bytes) {
			return (bytes + ALIGN - 1) & ~(ALIGN - 1);
		}

		union obj {
			union obj *free_list_link;
			char client_data[1];
		};

		static obj* volatile free_list[NFREELISTS];

		static size_t freelist_index(size_t bytes) {
			return (bytes - 1) / ALIGN + 1;
		}
		static void *refill(size_t n);
		static char *chunk_alloc(size_t size, int &nobjs);
		
		static char *start_free;
		static char *end_free;
		static size_t heap_size;
	};

	template <bool threads, int inst>
	char* alloc_optimize<threads, inst>::start_free = nullptr;

	template <bool threads, int inst>
	char* alloc_optimize<threads, inst>::end_free = nullptr;

	template <bool threads, int inst> 
	size_t alloc_optimize<threads, inst>::heap_size = 0;


	template <bool threads, int inst>
	typename alloc_optimize<threads, inst>::obj* volatile
	alloc_optimize<threads, inst>::free_list[NFREELISTS] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};

	template <bool threads, int inst>
	void* alloc_optimize<threads, inst>::allocate(size_t n) {
		if(n > MAX_BYTES) {
			return malloc_alloc::allocate(n);
		}

		obj* volatile *my_free_list;
		obj *result;
		
		my_free_list = free_list + freelist_index(n);
		result = *my_free_list;

		if(result == nullptr) {
			void *r = refill(round_up(n));
			return r;
		}
		*my_free_list = result->free_list_link;
		
		return result;
	}

	template <bool threads, int inst>
	void alloc_optimize<threads, inst>::deallocate(void *p, size_t n) {
		obj *q = static_cast<obj*>(p);
		obj *volatile *my_free_list;

		if(n > static_cast<size_t>(MAX_BYTES)) {
			malloc_alloc::deallocate(p, n);
			return ;
		}
		my_free_list = free_list + freelist_index(n);
		q->free_list_link = *my_free_list;
		*my_free_list = q;
	}

	template <bool threads, int inst> 
	void* alloc_optimize<threads, inst>::refill(size_t n) {
		int nobjs = 20;

		char *chunk = chunk_alloc(n, nobjs);
		obj *volatile *my_free_list;
		obj *result;
		if(nobjs == 1) {
			return chunk;	
		}
		obj *cur_obj;
		my_free_list = free_list + freelist_index(n);
		result = *my_free_list = cur_obj = static_cast<obj*>(chunk);
		for(int i = 1; i != nobjs; ++i) {
			cur_obj->free_list_link = static_cast<obj*>(static_cast<char*>(cur_obj) + n);
			cur_obj = cur_obj->free_list_link;		
		}
		cur_obj->free_list_link = nullptr;
		return result;
	}

	template <bool threads, int inst>
	char* alloc_optimize<threads, inst>::chunk_alloc(size_t size, int &nobjs) {
		char *result;
		size_t total_bytes = size * nobjs;
		size_t bytes_left = end_free - start_free;
		if(bytes_left >= total_bytes) {
			result = start_free;
			start_free += total_bytes;
			return result;
		} else if(bytes_left >= size) {
			nobjs = bytes_left / size;
			total_bytes = size * nobjs;
			result = start_free;
			start_free += total_bytes;
			return result;
		} else {
			size_t bytes_to_get = 2 * total_bytes + round_up(heap_size >> 4);
			if(bytes_left >= 0) {
				obj* volatile *my_free_list = free_list + freelist_index(bytes_left);
				static_cast<obj*>(start_free) -> free_list_link = *my_free_list;
				*my_free_list = static_cast<obj*>(start_free);
			}

			start_free = (char*)malloc(bytes_to_get);
			if(start_free == nullptr) {
				int i;
				obj* volatile *my_free_list, *p;
				for(i = size; i <= MAX_BYTES; i += ALIGN) {
					my_free_list = free_list + freelist_index(i);
					p = *my_free_list;
					if(p != nullptr) {
						*my_free_list = p->free_list_link;
						start_free = (char*)p;
						end_free = start_free + i;
						return chunk_alloc(size, nobjs);
					}
				}
				end_free = nullptr;
				start_free = (char*)malloc_alloc::allocate(bytes_to_get);
			}
			heap_size += bytes_to_get;
			end_free = start_free + bytes_to_get;
			return chunk_alloc(size, nobjs);
		}
	}

	typedef alloc_optimize<0, 0> alloc;
	
}


#endif

