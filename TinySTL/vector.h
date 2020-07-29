/*vector*/
/*新增C++11新特性，右值引用，移动构造，和花括号构造函数*/

#ifndef VECTOR_H
#define VECTOR_H

#include <cstddef>
//#include "construct.h"
//#include "alloc.h"
//#include "allocator.h"
#include <exception>			
#include <stdexcept>
#include <algorithm>  /*之后换成自己写的*/
#include <memory> /*先把问题聚集在容器上面*/

/*原本想着用自己写的allocator,发现bug多，为了将精力放在容器的实现上面,*/
/*配置器后面fix bug */
/*2020.7.14*/

/*对于异常安全函数，提供三个保证　基本承诺，强烈保证，不抛出异常*/

namespace mystl {
	template <typename T, typename Alloc = std::allocator<T> > 
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
	private:
		typedef Alloc data_allocator;
		//std::allocator<value_type> data_allocator;
		iterator elements;
		iterator first_free;
		iterator cap;

		iterator allocate(difference_type num) {
			return data_allocator.allocate(num);
		}
		void deallocate() {
			if(elements) data_allocator.deallocate(elements, cap - elements);
		}

		void destroy(iterator first, iterator last) {
			for(; first != last; ++first) {
				data_allocator.destroy(first, last);
			}
		}

		void destroy(iterator pos) {
			if(pos == nullptr) return ;
			data_allocator.destroy(pos);

		}

		bool check_size()
				{return cap == first_free;}

		void free() {
			destroy(elements, first_free)	;
			deallocate();
		}
		
		iterator alloc_n_construct(const iterator first, const iterator last) {
			iterator start = allocate(last - first);
			std::uninitialized_copy(first, last, start); //std need to mystl;
			return start;
		}

		void construct(iterator dest, const value_type &value) {
			data_allocator.construct(dest, value);
		}

		void construct(iterator dest, value_type &&value) {
			data_allocator.construct(dest, std::move(value));
		}

		void reallocate() {
			size_type newcapacity = size() ? 2 * size(): 1;
			pointer newdata = data_allocator.allocate(newcapacity);
			iterator dest = newdata;
			iterator src = elements;
			for(size_type i = 0; i < size(); ++i) {
				//data_allocator.construct(dest++, std::move(*elements++))	;
				construct(dest++, std::move(*elements++));
			}
			free();
			elements = newdata;
			first_free = dest;
			cap = elements + newcapacity;
		}

	public:
		/*默认构造函数*/
		vector(): 
			elements(nullptr), first_free(nullptr), cap(nullptr) { }

		/*拷贝构造函数*/
		vector(const vector &rhs) {
			elements = alloc_n_construct(rhs.elements, rhs.first_free);
			first_free = elements + rhs.size() ;
			cap = elements + rhs.capacity();
		}
/*----------------------------------分割线--------------------------------------------*/
		/*不抛出异常保证*/
		vector(vector &&rhs) noexcept: elements(rhs.elements), first_free(rhs.first_free), cap(rhs.cap) {
			rhs.elements = rhs.first_free = rhs.cap = nullptr;
		}

		vector&& operator=(vector &&rhs) noexcept {
			if(this != &rhs) {
				free();
				elements = rhs.elements;
				first_free = rhs.first_free;
				cap = rhs.cap;
				elements = first_free = cap = nullptr;
			}
		}
/*----------------------------2020.7.18解决移动构造函数和移动赋值运算符-------------------*/
		/*拷贝赋值运算符*/
		/*基本异常保证*/
		vector& operator=(const vector &rhs) {
			difference_type size = rhs.size();
			iterator start = alloc_n_construct(rhs.elements, rhs.first_free);
			free();
			elements = start;
			first_free = cap = start + size;
		}

		
		/*析构函数*/
		~vector() { free(); }


		/*普通构造函数*/
		vector(size_type n, const T &value) {
			elements = allocate(n);
			first_free = cap = elements + n;
			std::uninitialized_fill_n(elements, n, value);//need std to mystl
		}

		vector(iterator begin, iterator end) {
			elements = alloc_n_consturct(begin, end);
			first_free = cap = end;	
		}

		explicit vector(size_type n, T value = T()) {
			elements = allocate(n);
			first_free = cap = elements + n;
			std::uninitialized_fill_n(elements, n, value); //need std to mystl;
		}



		/*获取元素*/
		reference back() 
				{ return *(first_free - 1); }
		const reference back() const
	   			{ return *(first_free - 1); }
		reference front() 
				{ return *elements; }
		const reference front() const
				{ return *elements; }
		
		/*获取迭代器*/
		iterator begin()
				{return elements; }
		iterator end()
				{ return first_free; }	
		const iterator cbegin() const
				{ return elements; }
		const iterator cend() const
				{ return first_free; }

		/*获取容器大小*/
		size_type size() const
				{return first_free - elements;}
		bool empty() const
				{ return elements == first_free; }
		size_type capacity() const
				{ return cap - elements; }

		/*重载[]*/
		reference operator[](size_type n)
				{ return *(elements + n); }
		const_reference operator[](size_type n) const
				{ return *(elements + n);}

		/*增加和删除元素*/
		void push_back(const T &value) {
			if(check_size()) reallocate();
			//construct(first_free++, value);
			data_allocator.construct(first_free++, value);
		}
		void push_back(value_type &&value) {
			if(check_size()) reallocate();
            //construct(first_free++, std::move(value)) ;
			data_allocator.construct(first_free++, std::move(value)); //后面将改成mystl::move(value);
        }

		iterator insert(const_iterator position, const value_type &val) {
			if(check_size()) reallocate();
			difference_type n = first_free - position; //理论上不会为负值。
			iterator last = first_free, pre = last - 1;
			for(size_type i = 0; i < n; ++i) {
				construct(last, *pre);
				destroy(pre);
				--last;
				--pre;
			}
			construct(position, val);
			++first_free;
			return position;
		}

		//返回的是第一个插入的元素位置迭代器
		iterator insert(const_iterator position, size_type n, const value_type &val) {
			while(cap - first_free < n) reallocate();
			if(first_free - position < n) {
				first_free = std::uninitialized_copy_n(position, first_free - position, position + n);
				destroy(position, first_free);
				std::uninitialized_fill_n(position, n, val);	
				return position;
			}

			else {
				iterator premove = first_free, needcto = first_free + n;
				for(size_type i = 0; i < first_free - position; ++i) {
					construct(--needcto, *--premove);
					destroy(premove);	
				}
				std::uninitialized_fill_n(position, n, val);
			}
			return position;
		}
/*-------------------------------分割线----------------------------------------------*/
		template <class InputIterator>
		iterator insert(const_iterator position, InputIterator first, InputIterator last) {
			size_type i = 0;
			for(; first != last; ++first) {
				insert(position + i, *first);
			}
			return position;
		}

		iterator insert(const_iterator position, std::initializer_list<value_type> il) {
			size_type i = 0;
			for(/*std::initializer_list::iterator*/auto  begin = il.begin(); begin != il.end(); ++begin) {
				insert(position + i, *begin);
			}
		}	//need std to mystl

		iterator insert(const_iterator position, value_type &&val) {
			if(check_size()) reallocate();
			difference_type n = first_free - position; //理论上不会为负值。
			iterator last = first_free, pre = last - 1;
			for(size_type i = 0; i < n; ++i) {
				construct(last, *pre);
				destroy(pre);
				--last;
				--pre;
			}
			data_allocator.allocate(position, std::move(val));
			++first_free;
		}

/*------------2020.7.18解决以上三个函数，包括first指向的类型不是容器类型,移动插入-----------------*/
		void pop_back() {
			if(empty()) throw std::runtime_error("underflow_error");
			data_allocator.destroy(--first_free);
		}

		iterator erase(iterator pos) {
			if(pos + 1 != end) {
				std::copy(pos + 1, first_free, pos);
			}
			destroy(--first_free);
			return pos;
		}

		iterator erase(iterator first, iterator last) {
			iterator i = std::copy(last, first_free, first); //need std to mystl
			destroy(i, first_free);
			first_free = first_free - last + first;
			return first;
		}

		void resize(size_type new_size, const value_type &X) {
			if(new_size > this->size()) {
				for(size_type i = 0; i < this->size() - new_size; ++i) {
					this->push_back(X);
				}
			}
			else if(new_size < this->size()) {
				for(size_type i = 0; i < new_size - this->size(); ++i) {
					this->pop_back();
				}
			}
		}

		void resize(size_type new_size, value_type &&X) {
			if(new_size > this->size()) {
				for(size_type i = 0; i < this->size() - new_size; ++i) {
					this->push_back(X);
				}
			}
			else if(new_size < this->size()) {
				for(size_type i = 0; i < new_size - this->size(); ++i) {
					this->pop_back();
				}
			}
		}

		void resize(size_type new_size, value_type X = value_type()) {
			if(new_size > this->size()) {
				for(size_type i = 0; i < this->size() - new_size; ++i) {
					this->push_back(X);
				}
			}
			else if(new_size < this->size()) {
				for(size_type i = 0; i < new_size - this->size(); ++i) {
					this->pop_back();
				}
			}
		}

		void swap(vector &x);
		void clear() {
			erase(first_free, elements);
		}

		//重载运算符

		bool operator<(const vector &rhs) const {
			for(size_type i = 0, j = 0; ;) {
				if(i >= size() && j >= rhs.size()) {
					break;
				}
				else if((i >= size() && j < size()) || *this[i] < rhs[j]) {
					return true;
				}
				else if((j >= size() && i < size()) || *this[i] >= rhs[j]) {
					break;
				}
				else {
					++i;
					++j;
				}
			}
			return false;
		}
		bool operator>(const vector &rhs) const {
			return rhs < *this;
		}

		bool operator<=(const vector &rhs) const {
			return !(rhs < *this);
		}

		bool operator>=(const vector &rhs) const {
			return !(*this < rhs);
		}

		bool operator!=(const vector &rhs) const {
			return *this < rhs || rhs < *this;
		}

		bool operator==(const vector &rhs) const {
			return !(*this < rhs) && !(rhs < *this);
		}
	};

}

#endif
