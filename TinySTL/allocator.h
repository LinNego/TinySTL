/*第一个项目的第一个文件*/
/*STL的allocater*/
/*allocater.h*/
#ifndef ALLOCATER_H
#define ALLOCATER_H

#include <cstddef>  //size_t, ptrdiff_t
#include <new>  //operator new ,delete
#include <cstdlib>
#include <cstdio>


namespace mystl {
	//命名加_的原因不让allocator成员函数产生递归调用	
	template <typename T>
	T*  _allocate(size_t size, T*) {
		T *t = static_cast<T*>(::operator new(size * sizeof(T)));
		if(t == nullptr) {
			fprintf(stderr, "out of memory\n");
			exit(0);
		}
		return t;
	}

	template <typename T>
	void _deallocate(T*p, size_t size) {
		for(size_t i = 0; i != size; ++i) {
			::operator delete(p + i);
		}
	}
		

	template <typename U, typename T>
	void _construct(U *p, const T &value) {  
		new (p) T(value);
	}

	template <typename T>
	void _destroy(T *p) {
		p->~T();
	}
			

	
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
		};
		allocator() = default;
		allocator(const allocator&);
			
		template <class U>
		allocator(const allocator<U>&);
		~allocator() { }
		
		pointer address(reference x) const { return &x; }
		const_pointer address(const_reference x) const {return &x; }
		pointer allocate(size_type n, const void *p = nullptr);
		//pointer allocate(size_type n);
		void deallocate(pointer p, size_type n);
		size_type max_size() const;
		void construct(pointer p, const T& x);
		void destroy(pointer p);
	};
	template <typename T>
	T* allocator<T>::allocate(size_type n, const void *p) {
		__allocate(n, static_cast<T*> (nullptr));
	}

	template <typename T>
	void allocator<T>::construct(pointer p, const T& x) {
		_construct(p, x);
	}

	template <typename T>
	void allocator<T>::destroy(pointer p) {
		_destroy(p);
	}

	template <typename T>
	void allocator<T>::deallocate(pointer p, size_type size) {
			__deallocate(p, size);
	}			
}

#endif
