/*第一个项目的第一个文件*/
/*STL的allocater*/
/*allocater.h*/
#ifndef ALLOCATER_H
#define ALLOCATER_H

#include <cstddef>


namespace mystl {
	template <typename T>
	class allocator {
		typedef T value_type;
		typedef T* pointer;
		typedef const T* const_pointer;
		typedef T& reference;
		typedef const T& const_reference;
		typedef size_t size_type;
		typedef ptrdiff_t different_type;
	public:
		template <typename U>
		class rebind {
		public:
			typedef allocator<U> other;
		}
		allocator();
		allocator(const allocator&);
		template <class U>
		allocator(const allocator<U>&);
		~allocator();
		pointer address(reference x) const;
		const_pointer allocator::address(const_reference x) const;
		pointer allocate(size_type n, const void* = nullptr);
		void deallocate(pointer p, size_type n);
		size_type max_size() const;
		void construct(pointer p, const T& x);
		void destroy(pointer p);
	}



}

#endif
