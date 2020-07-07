/*vector*/
/*新增C++11新特性，右值引用，移动构造，和花括号构造函数*/

#ifndef VECTOR_H
#define VECTOR_H

#include <cstddef>
#include "construct.h"
#include "alloc.h"
#include "allocator.h"
#include <exception>			
#include <stdexcept>
#include <algorithm>  /*之后换成自己写的*/

namespace mystl {
	template <typename T, typename Alloc = alloc> 
	class vector {
	public:
		typedef T value_type;
		typedef value_type* pointer;
		typedef value_type* iterator;
		typedef const value_type* const_iterator;
		typedef value_type& reference;
		typedef const value_type& const_reference;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;
	protected:
		typedef simple_alloc<value_type, Alloc> data_allocator;
		iterator *elements;
		iterator *first_free;
		iterator *cap;
		void deallocate() {
			if(elements) data_allocator::deallocate(elements, cap - elements);
		}
		void fill_initialize(size_type n, const T& value) {
			elements = allocate_and_fill(n, value);
			first_free = elements + n;
			cap = first_free;
		}
	public:
		vector(): 
			elements(nullptr), first_free(nullptr), cap(nullptr) { }
		vector(const vector&);
		vector(vector&&);
		vector& operator=(const vector&);
		vector&& operator=(vector&&);
		~vector() { free(); }

		vector(size_type n, const T&);
		vector(int n, const T&);
		vector(long long n, const T&);
		explicit vector(size_type n);

		reference back() 
				{ return *(first_free - 1); }
		const reference back() const
	   			{ return *(first_free - 1); }
		reference front() 
				{ return *elements; }
		const reference front() const
				{ return *elements; }
		iterator begin()
				{return elements; }
		iterator end()
				{ return first_free; }	
		const iterator cbegin() const
				{ return elements; }
		const iterator cend() const
				{ return first_free; }
		size_type size() const
				{return first_free - elements;}
		size_type max_size() const;
		bool empty() const
				{ return elements == first_free; }
		size_type capacity() const
				{ return cap - elements; }
		reference operator[](size_type n)
				{ return *(elements + n); }
		const_reference operator[](size_type n) const
				{ return *(elements + n);}
		void resize(size_type new_size);
		void resize(size_type new_size, const value_type &X);
		void resize(size_type new_size, value_type X = value_type());
		void reserve(size_type n);
		void push_back(const T &value) {
			if(check_size()) reallocate();
			construct(first_free, value);
			++first_free;
		}
		void push_back(value_type&&);
		iterator insert(iterator pos, const T&, size_type n = 1);
		void emplace_back();
		void pop_back() {
			if(empty()) throw std::runtime_error("underflow_error");
			--first_free;
			destroy(first_free);
		}
		iterator erase(const_iterator pos);
		iterator erase(const_iterator first, const_iterator last);
		void swap(vector &x);
		void clear();
	protected:
		bool check_size()
				{return cap == first_free;}
		void free() {
			destroy(elements, first_free)	;
			deallocate();
		}
		void reallocate();
	};
	/*暂时不支持右值引用,也就是移动构造*/
	template <typename T, typename Alloc>
	void vector<T, Alloc>::reallocate() {
		size_type newcapacity = size() ? 2 * size(): 1;
		pointer newdata = data_allocator::allocate(newcapacity);
		iterator dest = newdata;
		iterator src = elements;
		for(size_type i = 0; i < size(); ++i) {
			data_allocator::construct(dest++, *elements++)	;
		}
		free();
		elements = newdata;
		first_free = dest;
		cap = elements + newcapacity;
	}

	template <typename T, typename Alloc>
	typename vector<T, Alloc>::iterator
	vector<T, Alloc>::erase(const_iterator pos) {
		if(pos + 1 != first_free) {
			copy(pos + 1, first_free, pos)	;  //algorithm的函数
		}
		--first_free;
		destroy(first_free);
		return pos;
	}
	
	template <typename T, typename Alloc>
	typename vector<T, Alloc>::iterator
	vector<T, Alloc>::erase(const_iterator first, const_iterator last) {
		iterator i = copy(last, first_free, first);
		destroy(i, first_free);
		first_free = first_free - (last - first);
		return first;
	}

	template <typename T, typename Alloc>
	typename vector<T, Alloc>::iterator
	vector<T, Alloc>::insert(iterator pos, const T &value, size_type n) {
		if(n != 0) {
			if(cap - first_free < n) reallocate();
			iterator newfirst_free = first_free + n, oldfirst_free = first_free;
			first_free = newfirst_free;
			for(size_type i = 0; i < n; ++i) {
				construct(--newfirst_free, *--oldfirst_free);
			}
			destroy(pos, pos + n);
			for(size_type i = 0; i < n; ++i) {
				construct(pos, value);
			}
			return pos;
			
		}
		else return pos;
	}

}

#endif
