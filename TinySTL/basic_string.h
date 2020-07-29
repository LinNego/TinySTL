//一个模板类　basic_string
//string的基础
#ifndef BASIC_STRING_H
#define BASIC_STRING_H

/*for c */
#include <cstddef>
#include <cstring>

/*for c++11　以后需要修改成自己头文件*/
#include <exception>
#include <memory>
#include <type_traits>
#include <vector>
#include <initializer_list>
#include <iterator>
#include <iostream>  //for test

#include "char_traits.h"


namespace mystl {

	/*初始每次分配100，之后都是2 * n + m*/
	template <typename charT, typename traits = std::char_traits<charT>, typename Alloc = std::allocator<charT> >
	class basic_string {
	public:
		typedef traits type_traits;
		typedef Alloc allocator_type;
		typedef typename type_traits::char_type value_type;
		typedef value_type& reference;
		typedef const value_type& const_reference;
		typedef typename std::allocator_traits<allocator_type>::pointer pointer;
		typedef typename std::allocator_traits<allocator_type>::const_pointer const_pointer;
	   	typedef charT* iterator;
		typedef const charT* const_iterator;
/*std*/ typedef std::reverse_iterator<iterator> reverse_iterator;
/*std*/ typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
		typedef typename std::allocator_traits<allocator_type>::difference_type difference_t;
		typedef typename std::allocator_traits<allocator_type>::size_type size_type; //maybe non-sign???
	public:
		static constexpr size_type npos = static_cast<size_type>(-1);
	private:
		allocator_type alloc;
		iterator elements;  //开始的位置
		size_type _size;    //现有
		size_type _cap;     //cap
	private:
		/*工具函数*/
		void copy_from(iterator pointer, const basic_string &str, size_type pos, size_type len) {
			for(int i = 0; i < len; ++i) {
				alloc.construct(pointer + i, str[pos + i]);
			}
		}

		void destroy(const_iterator b, size_type len) {
			for(int i = 0; i < len; ++i) {
				alloc.destroy(b++);
			}
		}

		void free() {
			destroy(elements, _size);
			alloc.deallocate(elements, _cap);
		}

		/*for iterator */
		template <typename InputIterator>
		iterator iterator_construct(InputIterator first, InputIterator last, size_type n) {
			iterator newstart = alloc.allocate(2 * n + 1);
			while(first != last) {
				alloc.construct(newstart++, *first++);
			}
			return newstart;
		}
		void GetNext(std::vector<charT> &Next, const basic_string &s) {
			Next[0] = -1;
			size_type len = s.size();
			difference_t i = 0, j = -1;
			while(i < len) {
				if(j == -1 || s[i] == s[j]) {
					i++;
					j++;
					Next[i] = j;	
				}
				else {
					j = Next[j];
				}
			}
		}

		void GetNext(std::vector<charT> &Next, const char *s) {
			Next[0] = -1;
			size_type len = strlen(s);
			difference_t i = 0, j = -1;
			while(i < len) {
				if(j == -1 || s[i] == s[j]) {
					i++;
					j++;
					Next[i] = j;
				}
				else {
					j = Next[j];
				}
			}
		}

		iterator reallocate(size_type len) {
			iterator newstart = alloc.allocate(len);
			memcpy(newstart, elements, _size);
			return newstart;
		}
		

	public:
	/*构造函数*/

		/*默认构造函数*/
		explicit basic_string(const allocator_type &alloc = allocator_type()): alloc(alloc), elements(this->alloc.allocate(100)), _size(0), _cap(100) { }

		/*拷贝构造函数*/
		basic_string(const basic_string &str): 
			alloc(str.alloc), elements(this->alloc.allocate(str._cap)), _size(str._size), _cap(str._cap){
                 //std::cout << "拷贝构造\n" << std::endl; //for test
				copy_from(elements, str, 0, str._size);
		}

		basic_string(const basic_string &str, const allocator_type& alloc): 
			alloc(alloc),elements(this->alloc.allocate(str._cap)), _size(str._size), _cap(str._cap) { 
				copy_from(elements, str, 0, str._size);
		}


		basic_string(const basic_string &str, size_type pos, size_type len = npos,const allocator_type &alloc = allocator_type()) :
			alloc(alloc) {
                 if(len == npos) len = str.size() - pos;
                 elements = this->alloc.allocate(len);
                 _size = len;
                 _cap = len;
				copy_from(elements, str, pos, len);
		}


		basic_string(const charT *s, const allocator_type& alloc = allocator_type()): 
			alloc(alloc), elements(this->alloc.allocate(strlen(s))), _size(strlen(s)), _cap(_size) {
				memcpy(elements, s, _size);
		}


		basic_string(const charT *s, size_type n, const allocator_type& alloc = allocator_type()): 
			alloc(alloc){
                size_type len = strlen(s);
                if(n > len) throw std::bad_alloc();
                elements = this->alloc.allocate(n);
                 _size = n;
                 _cap = n; 
				memcpy(elements, s, _size);
		}

		basic_string(size_type n, charT c, const allocator_type &alloc = allocator_type()):
						alloc(alloc), elements(this->alloc.allocate(n)), _size(n), _cap(n) {
			iterator it = elements;
			for(int i = 0; i < n; ++i) {
				this->alloc.construct(it++, c);
			}
		}
		template <typename InputIterator>
		basic_string(InputIterator first, InputIterator last, const allocator_type &alloc = allocator_type()):basic_string(alloc) {
			for(InputIterator i = first; i != last; ++i) {
				++_size;
			}
			_cap = _size;
			elements = this->alloc.allocate(_size);
			iterator i = elements;
			for(; first != last; ++first) {
				this->alloc.construct(i++, *first);
			}
		}

		basic_string(std::initializer_list<charT> il, const allocator_type& alloc = allocator_type()):
					alloc(alloc), elements(this->alloc.allocate(il.size())), _size(il.size()), _cap(_size) {
			iterator it = elements;
			for(auto &i: il) {
				this->alloc.construct(it++, i);
			}
		}

		basic_string(basic_string &&str) noexcept: 
						alloc(allocator_type()), elements(str.elements), _size(str._size), _cap(str._cap)  {
            //std::cout << *elements << std::endl; for test
			str.elements = nullptr;
			str._size = str._cap = 0;
             
		}


		basic_string(basic_string &&str, const allocator_type &alloc) : 
				alloc(alloc), elements(str.elements), _size(str._size), _cap(str._cap) {
			str.elements = nullptr;
			_size = _cap = 0;
		}

		~basic_string() {
			free();
		}

		basic_string& operator=(const basic_string& str) {
			iterator start = alloc.allocate(str._cap);
			copy_from(start, str, 0, str._size);
			free();
			elements = start;
			_size = str._size;
			_cap = str._cap;
			return *this;
		}

		basic_string& operator=(const charT *s) {
			size_t len = strlen(s);
			iterator start = alloc.allocate(len);
			memcpy(start, s, len);
			free();
             elements = start;
			_size = _cap = static_cast<size_type>(len);
			return *this;
		}

		basic_string& operator=(charT c) {
			iterator start = alloc.allocate(size());
             free();
             elements = start;
			*elements = c;
             _size = 1;
			return *this;
		}

		
		basic_string& operator=(std::initializer_list<charT> il) {
			size_type len = il.size();
			iterator start = alloc.allocate(len);
			iterator temp = start;
			for(auto &i: il) {
				alloc.construct(temp++, i);
			}
			free();
			elements = start;
			_size = _cap = len;
			return *this;
		}

		basic_string& operator=(basic_string &&str)noexcept {
			elements = str.elements;
			_size = str._size;
			_cap = str._cap;
			str.elements = nullptr;
			_size = _cap = 0;
			return *this;
		}
	
	public:
		/*return iterator*/
		iterator begin() noexcept { return elements; }
		const_iterator begin() const noexcept { return elements;}
		const_iterator cbegin() const noexcept { return elements; }	
		iterator end() noexcept { return elements + _size; }
		const_iterator end() const noexcept{ return elements + _size;}
		const_iterator cend() const noexcept { return elements + _size; }


		/*Capacity*/
		size_type size() const noexcept { return _size;}
		size_type length() const noexcept { return _size;}
		size_type max_size() const noexcept { return SIZE_MAX;}
		void resize(size_type n) {
			if(size() < n) {
				if(_cap < n) {
					iterator newstart = reallocate(2 * n + 1);
					free();
					elements = newstart;
					iterator f = elements + size();
					for(size_type i = 0; i < n - size(); ++i) {
						alloc.construct(f++, static_cast<charT>('\0'));						
					}
					_size = n;
					_cap = 2 * n + 1;
				}
				else {
					iterator f = elements + size();
					for(size_type i = 0; i < n - size(); ++i) {
						alloc.construct(f++, static_cast<charT>('\0'));						
					}
					_size = n;
				}
			}
			else {
				destroy(elements + n, size() - n); //what the hell n - size() ......
				_size = n;
			}
		}

		void resize(size_type n, charT ch) {
			if(size() < n) {
				if(_cap < n) {
					iterator newstart = reallocate(2 * n + 1);
					free();
					elements = newstart;
					iterator f = elements + size();
					for(size_type i = 0; i < n - size(); ++i) {
						alloc.construct(f++, ch);						
					}
					_size = n;
					_cap = 2 * n + 1;
				}
				else {
					iterator f = elements + size();
					for(size_type i = 0; i < n - size(); ++i) {
						alloc.construct(f++, ch);						
					}
					_size = n;
				}
			}
			else {
				destroy(elements + n, size() - n);
				_size = n;
			}
		}

		size_type capacity() const noexcept {
			return _cap;
		}

		void reserver(size_type n = 0) {
			if(n > capacity()) {
				iterator newstart = reallocate(2 * n + 1);
				free();
				elements = newstart;
				_cap = 2 * n + 1;
			}
		}

		void clear() noexcept {
			for(auto &i: *this)
				i = static_cast<charT>('\0');
		}

		bool empty() const noexcept {
			return _size == static_cast<size_type>(0);
		}

		void shrink_to_fit() noexcept {
			/*do nothing*/
		}

		/*element access元素访问*/
		reference operator[] (size_type pos) {
			return *(elements + pos);
		}
		const_reference operator[] (size_type pos) const {
			return *(elements + pos);
		}

		reference at (size_type pos) {
			return operator[](pos);
		}
		const_reference at (size_type pos) const {
			return operator[](pos);
		}

		reference back() {
			return *(elements + _size - 1);
		}

		const_reference back() const {
			return *(elements + _size + 1);
		}

		reference front() {
			return *(elements);
		}
		const_reference front() const {
			return *(elements);
		}

		/*Modifiers　修改*/
		/*string*/
		basic_string& operator+= (const basic_string& str) {
			size_type len = size() + str.size();
			if(len >= capacity()) {
				iterator newstart = reallocate(2 * len + 1);
				free();
				elements = newstart;
				newstart = elements + size();
				_size = len;
				_cap = 2 * len + 1;
				copy_from(newstart, str, 0, str.size());
			}
			else {
				copy_from(elements + size(), str, 0, str.size());
				_size = len;
			}
			
			return *this;
		}

		/*c-string*/
		basic_string& operator+= (const charT* s) {
			size_type len = strlen(s);
			if(len + size() >= capacity()) {
				iterator newstart = reallocate(2 * (len + size()) + 1);
				free();
				elements = newstart;
				memcpy(elements + size(), s, len);
				_cap = 2 * (len + size()) + 1;
				_size += len;
			}
			else {
				memcpy(elements + size(), s, len);
				_size += len; 
			}
			return *this;
		}

		/*character*/
		basic_string& operator+= (charT c) {
			if(capacity() < size()) {
				iterator newstart = reallocate(2 * (1 + size()) + 1);
				free();
				elements = newstart;
				alloc.construct(elements + size(), c);
				_size += 1;
			}
			else {
				alloc.construct(elements + size(), c);
				_size += 1;
			}
			return *this;
		}

		/*initializer list*/
		basic_string& operator+= (std::initializer_list<charT> il) {
			size_type len = size() + il.size();
			if(len >= capacity()) {
				iterator newstart = reallocate(2 * (len + size()) + 1);
				free();
				elements = newstart;
				std::uninitialized_copy(il.begin(), il.end(), elements + size()); //need std to mystl
				_cap = 2 * (len + size()) + 1;
				_size = len;
			}
			else {
				std::uninitialized_copy(il.begin(), il.end(), elements + size()); //need std to mystl
				_size = len;
			}
			return *this;
		}
			/*string*/
		basic_string& append (const basic_string& str) {
			return *this += str;
		}
			/*substring*/
		basic_string& append (const basic_string& str, size_type subpos, size_type sublen) {
			size_type len = sublen + size();
			if(len >= capacity()) {
				iterator newstart = reallocate(2 * len + 1);
				free();
				elements = newstart;
				copy_from(elements + size(), str, subpos, sublen);
				_cap = 2 * len + 1;
				_size = len;
			}	
			else {
				copy_from(elements + size(), str, subpos, sublen);
				//std::cout << "test" << std::endl; //for test
				_size = len;
			}
			return *this;
		}
			/*c-string*/
		basic_string& append (const charT* s) {
			return *this += s;
		}
			/*buffer*/
		basic_string& append (const charT* s, size_type n) {
			for(size_type i = static_cast<size_type>(0); i < n; ++i) {
				*this += s[i];
			}
			return *this;
		}
			/*fill*/
		basic_string& append (size_type n, charT c) {
			for(size_type i = static_cast<size_type>(0); i < n; ++i) {
				*this += c;
			}
			return *this;
		}
			/*range*/
		template <typename InputIterator>
		basic_string& append (InputIterator first, InputIterator last) {
			InputIterator i = first;
			size_type len = 0;
			while(i++ != last) ++len;
			if(len + size() >= capacity()) {
				iterator newstart = reallocate(2 * (len + size()));
				elements = newstart;
				newstart = elements + size();
				while(first != last) {
					alloc.construct(newstart++, *first++);
				}
				_cap = 2 * (len + size());
				_size += len;
				return *this;
			}
			else {
				std::cout << "test" << std::endl;//for test
				iterator s = elements + size();
				while(first != last) {
					alloc.construct(s++, *first++);
				}
				_size += len;
			}
			return *this;
		}
			/*initializer list*/
		basic_string& append (std::initializer_list<charT> il) {
			*this += il;
			return *this;
		}

		void push_back(charT ch) {
			*this += ch;
		}
			/*string*/
		basic_string& assign (const basic_string& str) {
			*this = str;
			return *this;
		}
			/*substring*/
		basic_string& assign (const basic_string& str, size_type subpos, size_type sublen) {
			iterator newstart = alloc.allocate(sublen);
			copy_from(newstart, str, subpos, sublen);
			free();
			elements = newstart;
			_size = _cap = sublen;
			return *this;
		}
			/*c-string*/
		basic_string& assign (const charT* s) {
			*this = s;
			return *this;
		}
			/*buffer*/
		basic_string& assign (const charT* s, size_type n) {
			iterator newstart = alloc.allocate(n);
			memecpy(newstart, s + n, n);
			free();
			elements = newstart;
			_size = _cap = n;
			return *this;
		}
			/*fill*/
		basic_string& assign (size_type n, charT c) {
			iterator newstart = alloc.allocate(n);
			for(size_type i = 0; i < n; ++i) {
				alloc.construct(newstart++, c);
			}
			free();
			elements = newstart;
			_size = _cap = n;
			return *this;
		}
			/*range*/
		template <typename InputIterator>
		basic_string& assign(InputIterator first, InputIterator last) {
			InputIterator i = first;
			size_type len = 0;
			while(i++ != last) ++len;
			iterator newstart = iterator_construct(first, last);
			free();
			elements = newstart;
			_size = len;
			_cap = 2 * len + 1;
			return *this;

		}
			/*initializer list*/
		basic_string& assign(std::initializer_list<charT> il) {
			*this = il;
			return *this;
		}
			/*move*/
		basic_string& assign(basic_string&& str) noexcept {
			*this = str;
			return *this;
		}
			/*string*/
		basic_string& insert (size_type pos, const basic_string& str) {
			size_type len = str.size() + size();
			size_type backnum = _size - pos;
			if(len >= capacity()) {
				iterator newstart = reallocate(2 * len + 1);
				free();	
				elements = newstart;
				_cap = 2 * len + 1;
			}
			memmove(elements + pos + str.size(), elements + pos, backnum);
			iterator cur = elements + pos;
			destroy(cur, str.size());
			for(size_type i= 0; i < str.size(); ++i) {
				alloc.construct(cur++, str[i]);
			}
			_size += str.size();
			return *this;
		}
			/*substring*/
		basic_string& insert (size_type pos, const basic_string& str,
							size_type subpos, size_type sublen) {
            size_type len = sublen + size(); 
			size_type backnum = _size - pos;
			if(len >= capacity()) {
				iterator newstart = reallocate(2 * len + 1);
				free();	
				elements = newstart;
				_cap = 2 * len + 1;
			}
			memmove(elements + pos + sublen, elements + pos, backnum);
			iterator cur = elements + pos;
			destroy(cur, sublen);
			for(size_type i = 0; i < sublen; ++i) {
				alloc.construct(cur++, str[subpos + i]);
			}
			_size += sublen;
			return *this;
		}
			/*c-string*/
		basic_string& insert (size_type pos, const charT* s) {
			size_type sublen = strlen(s);
            size_type len = sublen + size(); 
			size_type backnum = _size - pos;
			if(len >= capacity()) {
				iterator newstart = reallocate(2 * len + 1);
				free();	
				elements = newstart;
				_cap = 2 * len + 1;
			}
			memmove(elements + pos + sublen, elements + pos, backnum);
			iterator cur = elements + pos;
			destroy(cur, sublen);
			for(size_type i = 0; i < sublen; ++i) {
				alloc.construct(cur++, s[i]);
			}
			_size += sublen;
			return *this;

		}
			/*buffer*/
		basic_string& insert (size_type pos, const charT* s, size_type n) {
			size_type sublen = n;
            size_type len = sublen + size();
			size_type backnum = _size - pos;
			if(len >= capacity()) {
				iterator newstart = reallocate(2 * len + 1);
				free();	
				elements = newstart;
				_cap = 2 * len + 1;
			}
			memmove(elements + pos + sublen, elements + pos, backnum);
			iterator cur = elements + pos;
			destroy(cur, sublen);
			for(size_type i = 0; i < sublen; ++i) {
				alloc.construct(cur++, s[i]);
			}
			_size += sublen;
			return *this;
		}
			/*fill*/
		basic_string& insert (size_type pos, size_type n, charT c) {
			size_type sublen = n;
            size_type len = sublen + size();
			size_type backnum = _size - pos;
			if(len >= capacity()) {
				iterator newstart = reallocate(2 * len + 1);
				free();	
				elements = newstart;
				_cap = 2 * len + 1;
			}
			memmove(elements + pos + sublen, elements + pos, backnum);
			iterator cur = elements + pos;
			destroy(cur, sublen);
			for(size_type i = 0; i < sublen; ++i) {
				alloc.construct(cur++, c);
			}
			_size += sublen;
			return *this;

		}
		iterator insert (iterator p, size_type n, charT c) {
			size_type len = n + size();
			size_type backnum = elements + _size - p, prenum = p - elements;
			if(len >= capacity()) {
				iterator newstart = reallocate(2 * len + 1);
				free();	
				elements = newstart;
				_cap = 2 * len + 1;
			}
			memmove(elements + prenum + n, elements + prenum, backnum);
			iterator ret = elements + prenum;
			destroy(ret, n);
			for(size_type i = 0; i < n; ++i) {
				alloc.construct(ret + i, c);
			}
			_size += n;
			return ret;
		}
			/*single character*/
		iterator insert (iterator p, charT c) {
			return insert(p, 1, c);
		}
			/*range*/
		template <class InputIterator>
		iterator insert (iterator p, InputIterator first, InputIterator last) {
			size_type n = 0;
			InputIterator temp = first;
			while(temp != last) ++n;
			size_type len = n + size();
			size_type backnum = elements + _size - p, prenum = p - elements;
			if(len >= capacity()) {
				iterator newstart = reallocate(2 * len + 1);
				free();	
				elements = newstart;
				_cap = 2 * len + 1;
			}
			memmove(elements + prenum + n, elements + prenum, backnum);
			iterator ret = elements + prenum;
			destroy(ret, n);
			for(size_type i = 0; i < n; ++i) {
				alloc.construct(ret + i, *first++);
			}
			_size += n;
			return ret;

		}
			/*initializer list*/
		basic_string& insert (iterator p, std::initializer_list<charT> il)  {
			size_type len = il.size() + size();
			size_type backnum = elements + _size - p, prenum = p - elements;
			if(len >= capacity()) {
				iterator newstart = reallocate(2 * len + 1);
				free();	
				elements = newstart;
				_cap = 2 * len + 1;
			}
			memmove(elements + prenum + il.size(), elements + prenum, backnum);
			iterator ret = elements + prenum;
			destroy(ret, il.size());
			for(size_type i = 0; i < il.size(); ++i) {
				alloc.construct(ret + i, il[i]);
			}
			_size += il.size();
			return ret;
		}

			/*sequence*/
		basic_string& erase(size_type pos = 0, size_type len = npos) {
			if(empty()) return *this;
			destroy(elements + pos, len);
			iterator start = elements + pos, last = elements + pos + len;
			while(len--) {
				alloc.construct(start++, *last);
				alloc.destroy(last++);
			}
			_size -= len;
			return *this;
		}
			/*character*/
		iterator erase (iterator p) {
			if(p == end())	return end();
			if(empty()) return end();
			iterator ret = p;
			alloc.destroy(p);
			iterator next = p + 1;
			while(p != end()) {
				alloc.construct(p, *next);
				++p;
				alloc.destroy(next++);
			}
			return ret;
		}
			/*range (3)*/
		iterator erase (iterator first, iterator last) {
			if(first == end()) return end();
			if(empty()) return end();
			difference_t len = last - first;
			destroy(first, len);
			difference_t num = end() - last;
			memmove(first, last, num);
			return first;
		}
		
				/*string*/
		basic_string& replace (size_type pos, size_type len, const basic_string& str) {
			if(pos >= size()) throw std::out_of_range();
			size_type backnum = _cap - pos, destroy_num;
			if(len > backnum) {
				iterator newstart = reallocate(2 * (len + pos) + 1);
				elements = newstart;
				_cap = 2 * (len + pos) + 1;
				if(_cap > max_size()) throw std::length_error();
			}
			destroy_num = min(len, size() - pos);
			if(len > size() - pos) _size = size + len - size() + pos;
			destroy(pos, destroy_num);
			iterator cur = elements + pos;
			for(size_type i = 0; i < len; ++i) {
				alloc.construct(cur++, str[i]);
			}	
			return *this;	
		}
		basic_string& replace (iterator i1, iterator i2, const basic_string& str) {
			(*this).erase(i1, i2);
			(*this).insert(i1 - elements, str);
			return *this;
		}

				/*substring*/
		basic_string& replace (size_type pos, size_type len, const basic_string& str,
							size_type subpos, size_type sublen) {
            (*this).erase(pos, len);
			(*this).insert(pos, str, subpos, sublen);
			return *this;
		}
				/*c-string*/
		basic_string& replace (size_type pos, size_type len, const charT* s) {
			(*this).erase(pos, len);
			(*this).insert(pos, s);
			return *this;
		}

		basic_string& replace (iterator i1, iterator i2, const charT* s) {
			(*this).erase(i1, i2);
			(*this).insert(i1 - elements, s);
			return *this;
		}
				/*buffer*/
		basic_string& replace (size_type pos, size_type len, const charT* s, size_type n) {
			(*this).erase(pos, len);
			(*this).insert(pos, s, n);
			return *this;
		}
		basic_string& replace (iterator i1, iterator i2, const charT* s, size_type n) {
			(*this).erase(i1, i2);
			(*this).insert(i1, s, n);
			return *this;
		}
				/*fill*/
		basic_string& replace (size_type pos, size_type len, size_type n, charT c) {
			(*this).erase(pos, len);
			(*this).insert(pos, n, c);
			return *this;
		}
		basic_string& replace (iterator i1, iterator i2, size_type n, charT c) {
			(*this).erase(i1, i2);
			(*this).insert(i1, n, c);
			return *this;
		}
				/*range*/
		template <class InputIterator>
		basic_string& replace (iterator i1, iterator i2,
							InputIterator first, InputIterator last) {
            (*this).erase(i1, i2);
			(*this).insert(i1, first, last);
			return *this;
		}
				/*initializer list*/
		basic_string& replace (iterator i1, iterator i2, std::initializer_list<charT> il) {
			(*this).erase(i1, i2);
			(*this).insert(i1, il);
			return *this;
		}


		void swap (basic_string& str) {
			iterator temp = str.elements;
			str.elements = elements;
			elements = temp;
			swap(_size, str._size);
			swap(_cap, str._cap);
		}

		void pop_back() {
			--_size;
			alloc.destroy(elements + _size);
		}

		/*String operations*/

		const charT* c_str() const noexcept {
			/*maybe not true*/
			alloc.construct(elements + size(), static_cast<charT>('\0'));
			return elements;
		}

		const charT* data() const noexcept {
			return c_str();
		}

		allocator_type get_allocator() const noexcept {
			return alloc;
		}

		size_type copy (charT* s, size_type len, size_type pos = 0) const {
			size_type ret = _size - pos;
			if(ret > len) {
				ret = len;
			}
			for(size_type i = 0; i < ret; ++i) {
				s[i] = *this[pos++];
			}
			return ret;
		}

				/*string*/
		size_type find (const basic_string& str, size_type pos = 0) const noexcept {
			std::vector<charT> Next(str.size(), 0);
			GetNext(Next, str);
			size_type i = pos, j = 0;
			while(i < size() && j < str.size()) {
				if(j == -1 || char_traits<charT>::eq((*this)[i], str[i])) {
					++i;
					++j;
				}
				else {
					j = Next[j];
				}
			}
			if(j == str.size()) {
				return i - j;
			}
			else return npos;
			
		}
				/*c-string*/
		size_type find (const charT* s, size_type pos = 0) const {
			size_type len = strlen(s);
			std::vector<charT> Next(len, pos);
			GetNext(Next, s);
			size_type i = pos, j = 0;
			while(i < size() && j < len) {
				if(j == -1 || char_traits<charT>::eq((*this)[i], s[i])) {
					++i;
					++j;
				}
				else {
					j = Next[j];
				}
			}
			if(j == len) {
				return i - j;
			}
			else return npos;
		}
				/*buffer*/
		size_type find (const charT* s, size_type pos, size_type n) const {
			size_type len = strlen(s);
			std::vector<charT> Next(len, 0);
			GetNext(Next, s);
			size_type i = pos, j = 0;
			while(i < pos + n && j < len) {
				if(j == -1 || char_traits<charT>::eq((*this)[i], s[j])) {
					++i;
					++j;
				}
				else {
					j = Next[j];
				}
			}
			if(j == len) {
				return i - n;
			}
			else return npos;
		}
				/*character*/
		size_type find (charT c, size_type pos = 0) const noexcept {
			while(pos < size()) {
				if(char_traits<charT>::eq((*this)[pos], c)) return pos;
				else ++pos;
			}	
			return npos;
		}
			/*string*/
		size_type rfind(const basic_string& str, size_type pos = npos) const noexcept {
			std::vector<charT> Next(str.size(), 0);
			size_type i = size() + pos, j = 0;
			while(i >= static_cast<size_type>(0) && j < str.size()) {
				if(j == -1 || char_traits<charT>::eq((*this)[i], str[j])) {
					--i;
					++j;
				}
				else {
					j = Next[j];
				}
			}
			if(j == str.size()) {
				return i + j;
			}
			else return npos;
		}
			/*c-string*/	
		size_type rfind (const charT* s, size_type pos = npos) const {
			size_type len = strlen(s);
			std::vector<charT> Next(len, 0);
			GetNext(Next, s);
			size_type i = size() + pos, j = 0;
			while(i >= static_cast<size_type>(0) && j < len) {
				if(j == -1 || char_traits<charT>::eq((*this)[i], s[j])) {
					--i;
					++j;
				}
				else {
					j = Next[j];
				}
			}
			if(j == len) {
				return i + j;
			}
			else return npos;
		}
			/*buffer*/
		size_type rfind (const charT* s, size_type pos, size_type n) const {
			size_type len = strlen(s);
			std::vector<charT> Next(len, 0);
			GetNext(Next, s);
			size_type i = size() + pos, j = 0;
			while(i >= size() - n && j < len) {
				if(j == -1 || char_traits<charT>::eq((*this)[i], s[j])) {
					--i;
					++j;
				}
				else {
					j = Next[j];
				}
			}
			if(j == len) {
				return i + j;
			}
			else return npos;

		}
			/*character*/
		size_type rfind (charT c, size_type pos = npos) const noexcept {
			if(pos == npos) pos = size();
			while(pos >= static_cast<size_type>(0)) {
				if(char_traits<charT>::eq(c, (*this)[pos])) {
					return pos;
				}
				else --pos;
			}
			return npos;
		}

			/*string*/
		size_type find_first_of (const basic_string& str, size_type pos = 0) const noexcept {
			size_type ret = npos;
			for(size_type i = 0; i < str.size(); ++i) {
				if((ret = find(str[i], pos)) != npos)  return ret;
			}
			return ret;
		}

			/*c-string*/
		size_type find_first_of (const charT* s, size_type pos = 0) const {
			size_type ret = npos, len = strlen(s);
			for(size_type i = 0; i < len; ++i) {
				if((ret = find(s[i], pos)) != npos) return ret;
			}
			return ret;
		}
		
		
			/*buffer*/
		size_type find_first_of (const charT* s, size_type pos, size_type n) const {
			size_type ret = npos;
			for(size_type i = 0; i < strlen(s); ++i) {
				size_type tpos = pos;
				while(tpos < pos + n) {
					if(char_traits<charT>::eq((*this)[tpos], s[i])) return tpos;
					++tpos;
				}
			}
			return ret;
		}
			/*character*/
		size_type find_first_of (charT c, size_type pos = 0) const noexcept {
			return find(c, pos);
		}
			/*string*/
		size_type find_last_of (const basic_string& str, size_type pos = npos) const noexcept {
			size_type ret = npos;
			for(size_type i = 0; i < str.size(); ++i) {
				if((ret = rfind(str[i], pos)) != npos)  return ret;
			}
			return ret;
		}
			/*c-string*/
		size_type find_last_of (const charT* s, size_type pos = npos) const {
			size_type ret = npos;
			size_type len = strlen(s);
			for(size_type i = 0; i < len; ++i) {
				if((ret = rfind(s[i], pos)) != npos)  return ret;
			}
			return ret;
		}
			/*buffer*/
		size_type find_last_of (const charT* s, size_type pos, size_type n) const {
			size_type len = strlen(s);
			for(size_type i = 0; i < len; ++i) {
				size_type tpos = pos;	
				while(tpos > pos - n); {
					if(char_traits<charT>::eq(s[i], (*this)[tpos])) return tpos;
					--tpos;
				}
			}
			return npos;
		}
			/*character*/
		size_type find_last_of (charT c, size_type pos = npos) const noexcept {
			return rfind(c, pos);
		}

			/*string*/
		size_type find_first_not_of (const basic_string& str, size_type pos = 0) const noexcept {
			for(size_type i = pos; i < size(); ++i) {
				if(str.find((*this)[i], 0) == npos) return i;
			}
			return npos;
		}
			/*c-string*/
		size_type find_first_not_of (const charT* s, size_type pos = 0) const {
			basic_string temp(s);
			for(size_type i = pos; i < size(); ++i) {
				if(temp.find((*this)[i], 0) == npos) return i;
			}
			return npos;
		}

			/*buffer*/
		size_type find_first_not_of (const charT* s, size_type pos, size_type n) const {
			basic_string temp(s);
			for(size_type i = pos; i < pos + n; ++i) {
				if(temp.find((*this)[i], 0) == npos) return i;
			}
			return npos;
		}
			/*character*/
		size_type find_first_not_of (charT c, size_type pos = 0) const noexcept {
			for(size_type i = pos; i < size(); ++i) {
				if(char_traits<charT>::eq(c, (*this)[i]) == npos) return i;
			}
			return npos;

		}

			/*string*/
		size_type find_last_not_of (const basic_string& str, size_type pos = npos) const noexcept {
			if(pos == npos) pos = size() - 1;
			for(size_type i = pos; i >= 0; --i) {
				if(str.find((*this)[i], 0) == npos) return i;
			}
			return npos;
		}
			/*c-string*/
		size_type find_last_not_of (const charT* s, size_type pos = npos) const {
			basic_string str(s);
			if(pos == npos) pos = size() - 1;
			for(size_type i = pos; i >= 0; --i) {
				if(str.find((*this)[i], 0) == npos) return i;
			}
			return npos;
		}
			/*buffer*/
		size_type find_last_not_of (const charT* s, size_type pos, size_type n) const {
			basic_string str(s);
			if(pos == npos) pos = size() - 1;
			for(size_type i = pos; i > pos - n; --i) {
				if(str.find((*this)[i], 0) == npos) return i;
			}
			return npos;
		}
			/*character*/
		size_type find_last_not_of (charT c, size_type pos = npos) const noexcept {
			if(pos == npos) pos = size() - 1;
			for(size_type i = pos; i >= 0; -i) {
				if(char_traits<charT>::eq(c, (*this)[i])) return i;
			}
			return npos;
		}

		basic_string substr(size_type pos = 0, size_type len = npos) {
			basic_string ret(*this, pos, len);
			return ret;
		}
			/*string*/
		int compare(const basic_string& str) const noexcept {
			if(size() != str.size()) return -1;
			for(size_type i = 0; i < size(); ++i) {
				if(!char_traits<charT>::eq((*this)[i], str[i])) {
					if((*this)[i] > str[i]) return 1;
					else return -1;
				}
			}	
			return 0;
		}
			/*substrings*/
		int compare(size_type pos, size_type len, const basic_string& str) const {
			if(len != str.size()) return -1;
			for(size_type i = 0; i < size(); ++i) {
				if(!char_traits<charT>::eq((*this)[pos + i], str[i])) {
					if((*this)[pos + i] > str[i]) return 1;
					else return -1;
				}
			}
			return 0;
		}
		int compare(size_type pos, size_type len, const basic_string& str,
					size_type subpos, size_type sublen) const {
            if(len != sublen) return -1;
			for(size_type i = 0; i < len; ++i) {
				if(!char_traits<charT>::eq((*this)[i + pos], str[i + subpos])) {
					if((*this)[pos + i] > str[i + subpos]) return 1;
					else return -1;
				}
			}
			return 0;
		}
			/*c-string*/
		int compare(const charT* s) const {
			size_type len = strlen(s);
			if(len != size()) return -1;
			for(size_type i = 0; i < len; ++i) {
				if(!char_traits<charT>::eq((*this)[i], s[i])) {
					if((*this)[i] > s[i]) return 1;
					else return -1;
				}
			}
			return 0;
		}
		int compare(size_type pos, size_type len, const charT* s) const {
			size_type len2 = strlen(s);
			if(len != len2) return -1;
			for(size_type i = 0; i < len; ++i) {
				if(!char_traits<charT>::eq((*this)[i + pos], s[i])) {
					if((*this)[i + pos] > s[i]) return 1;
					else return -1;
				}
			}
			return 0;
		}
			/*buffer*/
		int compare(size_type pos, size_type len, const charT* s, size_type n) const {
			if(len != n) return -1;
			for(size_type i = 0; i < len; ++i) {
				if(!char_traits<charT>::eq((*this)[i], s[i])) {
					if((*this)[i] > s[i]) return 1;
					else return -1;
				}
			}
			return 0;
		}
	};

	/*以下是非成员函数*/
		/*string*/
		template <class charT, class traits, class Alloc>
		basic_string<charT, traits, Alloc> operator+(const basic_string<charT,traits,Alloc>& lhs,
							const basic_string<charT,traits,Alloc>& rhs) {
			basic_string<charT,	traits, Alloc> ret(lhs);
			ret += rhs;
			return ret;
		}
	
		template <class charT, class traits, class Alloc>
		basic_string<charT, traits, Alloc> operator+(basic_string<charT,traits,Alloc>&& lhs,
							basic_string<charT,traits,Alloc>&& rhs) {
            basic_string<charT, traits, Alloc> ret(lhs);
			ret += rhs;
			return ret; 
		}
	
		template <class charT, class traits, class Alloc>
		basic_string<charT, traits, Alloc> operator+(basic_string<charT,traits,Alloc>&& lhs,
								const basic_string<charT,traits,Alloc>& rhs) {
		    basic_string<charT, traits, Alloc> ret(lhs);
			ret += rhs;
			return ret;		
		}
		template <class charT, class traits, class Alloc>
		basic_string<charT, traits, Alloc> operator+(const basic_string<charT,traits,Alloc>& lhs,
							basic_string<charT,traits,Alloc>&& rhs) {
            basic_string<charT, traits, Alloc> ret(rhs);
			ret += lhs;
			return ret;
		}
		/*c-string*/
	
		template <class charT, class traits, class Alloc>
		basic_string<charT, traits, Alloc> operator+(const basic_string<charT,traits,Alloc>& lhs, 
														const charT* rhs) {
            basic_string<char, traits, Alloc> ret(lhs);
			ret += rhs;
			return ret;
		}
		template <class charT, class traits, class Alloc>
		basic_string<charT, traits, Alloc> operator+(basic_string<charT,traits,Alloc>&& lhs, 
														const charT* rhs) {
            basic_string<char, traits, Alloc> ret(lhs);
			ret += rhs;
			return ret;
		}
		template <class charT, class traits, class Alloc>
		basic_string<charT, traits, Alloc> operator+(const charT* lhs, 
								const basic_string<charT,traits,Alloc>&rhs) {
            basic_string<char, traits, Alloc> ret(rhs);
			ret += lhs;
			return ret;
		}
		template <class charT, class traits, class Alloc>
		basic_string<charT, traits, Alloc> operator+(const charT* lhs, 
											basic_string<charT,traits,Alloc>&&rhs) {
            basic_string<charT, traits, Alloc> ret(rhs);
			ret += lhs;
			return ret;
		}

		/*character*/
	
		template <class charT, class traits, class Alloc>
		basic_string<char, traits, Alloc> operator+(const basic_string<charT,traits,Alloc>& lhs, charT rhs) {
			basic_string<charT, traits, Alloc> ret(lhs);
			ret += rhs;
			return ret;
		}

		template <class charT, class traits, class Alloc>
		basic_string<charT, traits, Alloc> operator+(basic_string<charT,traits,Alloc>&& lhs, charT rhs) {
			basic_string<charT, traits, Alloc> ret(lhs);
			ret += rhs;
		}

		template <class charT, class traits, class Alloc>
		basic_string<charT, traits, Alloc> operator+(charT lhs, const basic_string<charT,traits,Alloc>& rhs) {
			basic_string<charT, traits, Alloc> ret(rhs);
			ret += lhs;
			return ret;
		}

		template <class charT, class traits, class Alloc>
		basic_string<charT, traits, Alloc> operator+(charT lhs, basic_string<charT,traits,Alloc>&& rhs) {
			basic_string<charT, traits, Alloc> ret(rhs);
			ret += lhs;
			return ret;
		}

			/*关系运算符*/
			/*(1)*/
		template <class charT, class traits, class Alloc>
 		bool operator<(const basic_string<charT,traits,Alloc>& lhs,
                   		const basic_string<charT,traits,Alloc>& rhs) {
            typename basic_string<charT, traits, Alloc>::size_type i = 0, j = 0;
			for(; ;) {
				if((i == lhs.size() && j != rhs.size()) || lhs[i] < rhs[i]) {
					break;
				}
				else if((j == rhs.size() && i != lhs.size()) || (j == rhs.size() && i == lhs.size()) || rhs[i] < lhs[i]) {
					return false;
				}
				else {
					++i;
					++j;
				}
			}
			return true;
		}

		template <class charT, class traits, class Alloc>
		bool operator<(const charT* lhs, const basic_string<charT,traits,Alloc>& rhs) {
			typename basic_string<charT, traits, Alloc>::size_type i = 0, j =0, len;
			len = static_cast<typename basic_string<charT, traits, Alloc>::size_type>(strlen(lhs));
			for(; ;) {
				if((i == len && j != rhs.size()) || lhs[i] < rhs[i]) {
					break;
				}
				else if((j == rhs.size() && i != len) || (j == rhs.size() && i == len) || rhs[i] < lhs[i]) {
					return false;
				}
				else {
					++i;
					++j;
				}
			}
			return true;
		}

		template <class charT, class traits, class Alloc>
		bool operator<(const basic_string<charT,traits,Alloc>& lhs, const charT* rhs) {
			return !(rhs < lhs);
		}


			/*(2)*/	
		template <class charT, class traits, class Alloc>
		bool operator== (const basic_string<charT,traits,Alloc>& lhs,
							const basic_string<charT,traits,Alloc>& rhs) {
            return !(lhs < rhs) && !(rhs < lhs);
		}

		template <class charT, class traits, class Alloc>
		bool operator== (const charT* lhs, const basic_string<charT,traits,Alloc>& rhs) {
			return !(lhs < rhs) && !(rhs < lhs);
		}

		template <class charT, class traits, class Alloc>
		bool operator==(const basic_string<charT,traits,Alloc>& lhs, const charT* rhs) {
			return !(lhs < rhs) && !(rhs < lhs);
		}



			/*(3)*/
		template <class charT, class traits, class Alloc>
		bool operator!=(const basic_string<charT,traits,Alloc>& lhs,
							const basic_string<charT,traits,Alloc>& rhs) {
			return lhs < rhs || rhs < lhs;
		}

		template <class charT, class traits, class Alloc>
		bool operator!=(const charT* lhs, const basic_string<charT,traits,Alloc>& rhs) {
			return lhs < rhs || rhs < lhs;
		}

		template <class charT, class traits, class Alloc>
		bool operator!=(const basic_string<charT,traits,Alloc>& lhs, const charT* rhs) {
		return lhs < rhs ||  rhs < lhs;
	}


		/*(4)*/
		template <class charT, class traits, class Alloc>
		bool operator<=(const basic_string<charT,traits,Alloc>& lhs,
								const basic_string<charT,traits,Alloc>& rhs) {
			return !(rhs < lhs);
		}

		template <class charT, class traits, class Alloc>
		bool operator<=(const charT* lhs, const basic_string<charT,traits,Alloc>& rhs) {
			return !(rhs < lhs);
		}

		template <class charT, class traits, class Alloc>
		bool operator<=(const basic_string<charT,traits,Alloc>& lhs, const charT* rhs) {
			return !(rhs < rhs);
		}



		/*(5)*/
		template <class charT, class traits, class Alloc>
		bool operator>(const basic_string<charT,traits,Alloc>& lhs,
							const basic_string<charT,traits,Alloc>& rhs) {
			return rhs < lhs;
		}

		template <class charT, class traits, class Alloc>
		bool operator>(const charT* lhs, const basic_string<charT,traits,Alloc>& rhs) {
			return rhs < lhs;	
		}

		template <class charT, class traits, class Alloc>
		bool operator>(const basic_string<charT,traits,Alloc>& lhs, const charT* rhs) {
			return rhs < lhs;
		}


			/*(6)*/
		template <class charT, class traits, class Alloc>
		bool operator>=(const basic_string<charT,traits,Alloc>& lhs,
							const basic_string<charT,traits,Alloc>& rhs) {
			return !(lhs < rhs);
		}
		template <class charT, class traits, class Alloc>
		bool operator>=(const charT* lhs, const basic_string<charT,traits,Alloc>& rhs) {
			return !(lhs < rhs);
		}
		template <class charT, class traits, class Alloc>
		bool operator>= (const basic_string<charT,traits,Alloc>& lhs, const charT* rhs) {
			return !(lhs < rhs);
		}


		template <class charT, class traits, class Alloc>
		void swap(basic_string<charT,traits,Alloc>& x,
						basic_string<charT,traits,Alloc>& y) {
			x.swap(y);
		}


		/*重载的符合标准的原型*/
		/************************************************************************************************/
		// template <class charT, class traits, class Alloc>
		// basic_istream<charT,traits>& operator>>(basic_istream<charT, traits> &is, basic_string<charT,traits,Alloc>& str);
		// template <class charT, class traits, class Alloc>
		// basic_ostream<charT,traits>& operator<<(basic_ostream<charT,traits>&os ,const basic_string<charT,traits,Alloc>& str);
		//同理还有getline
		/************************这个TinySTL并不是实现输入输出,所以不使用这个原型 ******************************/

		template <class charT, class traits, class Alloc>
		std::istream& operator>>(std::istream&is, basic_string<charT,traits,Alloc>& str) {
			charT ch;
			while(is >> ch) {
				str += ch;	
			}
			return is;
		}
		template <class charT, class traits, class Alloc>
		std::ostream& operator<<(std::ostream& os,const basic_string<charT,traits,Alloc>& str) {
				typename basic_string<charT, traits, Alloc>::size_type len = str.size(), i = 0;
				while(i < len) {
					os << str[i++];
				}
				return os;
			}
			template <class charT, class traits, class Alloc>
			std::istream& getline (std::istream& is, basic_string<charT,traits,Alloc>& str, charT delim) {
				charT ch;
				while(is >> ch && ch != delim) {
					str += ch;
				}
				return is;
			}
			template <class charT, class traits, class Alloc>
			std::istream& getline (std::istream&& is, basic_string<charT,traits,Alloc>& str, charT delim) {
				charT ch;
				while(is >> ch && ch != delim) {
				str += ch;
			}
			return is;
		}

		template <class charT, class traits, class Alloc>
		std::istream& getline (std::istream& is, basic_string<charT,traits,Alloc>& str) {
			charT ch;
			while(is >> ch && ch != static_cast<charT>('\n')) {
				str += ch;
			}
			return is;
		}

		template <class charT, class traits, class Alloc>
		std::istream& getline (std::istream&& is, basic_string<charT,traits,Alloc>& str) {
			charT ch;
			while(is >> ch && ch != static_cast<charT>('\n')) {
				str += ch;
			}
			return is;
		}
		


}

#endif
