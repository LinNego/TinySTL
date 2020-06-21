//SGI STL 的第二级分配器仿写
/*LinNego*/

#ifndef MY_STL_ALLOC_OPTIMIZE_H
#define MY_STL_ALLOC_OPTIMIZE_H

#include <cstddef>  //for size_t
#include "mystl_alloc_prime.h"
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
	
}


#endif

