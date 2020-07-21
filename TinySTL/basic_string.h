//一个模板类　basic_string
//string的基础
#ifndef BASIC_STRING_H
#define BASIC_STRING_H

/*for c */
#include <cstddef>
#include <cstring>

/*for c++11　以后需要修改成自己头文件*/
#include <memory>
#include <type_traits>
#include <initializer_list>

#include "char_traits.h"


namespace mystl {

	/*初始每次分配100，之后都是2 * 100 + 1*/
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
		typedef typename std::allocator_traits<allocator_type>::difference_type difference_t;
		typedef typename std::allocator_traits<allocator_type>::size_type size_type;
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

		iterator reallocate(size_type len) {
			iterator newstart = alloc.allocate(len);
			memcpy(newstart, elements, _size);
			return newstart;
		}
		

	public:
	/*构造函数*/

		/*默认构造函数*/
		explicit basic_string(const allocator_type &alloc = allocator_type()): alloc(alloc), elements(alloc.allocate(100)), _size(0), _cap(100) { }

		/*拷贝构造函数*/
		basic_string(const basic_string &str): 
			alloc(str.alloc), elements(alloc.allocate(str._cap), _size(str._size), _cap(str._cap)){
				copy_from(elements, str, 0, str._size);
		}

		basic_string(const basic_string &str, const allocator_type& alloc): 
			alloc(alloc),elements(alloc.allocate(str._cap)), _size(str._size), _cap(str._cap) { 
				copy_from(elements, str, 0, str._size);
		}


		basic_string(const basic_string &str, size_type pos, size_type len = npos,const allocator_type &alloc = allocator_type()) :
			alloc(alloc), elements(alloc.allocate(len)), _size(len), _cap(str._cap) {
				copy_from(elements, str, pos, len);
		}


		basic_string(const charT *s, const allocator_type& alloc = allocator_type()): 
			alloc(alloc), elements(alloc.allocate(strlen(s))), _size(strlen(s)), _cap(_size) {
				memcpy(elements, s, _size);
		}


		basic_string(const charT *s, size_type n, const allocator_type& alloc = allocator_type()): 
			alloc(alloc), elements(alloc.allocate(n)), _size(n), _cap(_size) {
				memcpy(elements, s, _size);
		}

		basic_string(size_type n, charT c, const allocator_type &alloc = allocator_type()):
						alloc(alloc), elements(alloc.allocate(n)), _size(n), _cap(n) {
			iterator it = elements;
			for(int i = 0; i < n; ++i) {
				alloc.construct(it++, c);
			}
		}
		template <typename InputIterator>
		basic_string(InputIterator first, InputIterator last, const allocator_type &alloc = allocator_type()):basic_string(alloc) {
			for(InputIterator i = first; i != last; ++i) {
				++_size;
			}
			_cap = _size;
			elements = alloc.allocate(_size);
			iterator i = elements;
			for(; first != last; ++first) {
				alloc.construct(i, *first);
			}
		}

		basic_string(std::initializer_list<charT> il, const allocator_type& alloc = allocator_type()):
						alloc(alloc), elements(alloc.allocate(il.size())), _size(il.size()), _cap(_size) {
			iterator it = elements;
			for(auto &i: il) {
				alloc.construct(it++, i);
			}
		}

		basic_string(basic_string &&str) noexcept: 
						alloc(allocator_type()), elements(str.elements), _size(str._size), _cap(str._cap)  {
			str.elements = nullptr;
			_size = _cap = 0;
		}


		basic_string(basic_string &&str, const allocator_type &alloc) : 
				alloc(allocator_type()), elements(str.elements), _size(str._size), _cap(str._cap) {
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
			~basic_string();
			_size = _cap = static_cast<size_type>(len);
			return *this;
		}

		basic_string& operator=(charT c) {
			destroy(elements, _size);
			*elements = c;
			return *this;
		}

		
		basic_string& operator=(std::initializer_list<charT> il) {
			size_type len = il.size();
			iterator start = alloc.allocate(len);
			iterator temp = start;
			for(auto &i: il) {
				alloc.construct(temp++, i);
			}
			~basic_string();
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
				if(max_size < n) {
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
				destroy(elements + n, n - size());
				_size = n;
			}
		}

		void resize(size_type n, charT ch) {
			if(size() < n) {
				if(max_size < n) {
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
				destroy(elements + n, n - size());
				_size = n;
			}
		}

		size_type capacity() const noexcept {
			return _cap;
		}

		void reserver(size_type n = 0) {
			if(n > capacity) {
				iterator newstart = reallocate(2 * n + 1);
				free();
				elements = newstart;
				_cap = 2 * n;
			}
		}

		void clear() noexcept {
			destroy(elements, size());
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
			if(!empty()) {
				return *(elements + _size - 1);
			}
		}

		const_reference back() const {
			if(!empty()) {
				return *(elements + _size + 1);
			}
		}

		reference front() {
			if(!empty()) return *(elements);
		}
		const_reference front() const {
			if(!empty()) return *(elements);
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
				copy_from(newstart, str, 0, str.size);
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
			if(len + size() >= capacity) {
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
			if(capacity < size()) {
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
				copy_from(elements, str, subpos, sublen);
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
				_size = len;
				return *this;
			}
			else {
				iterator s = elements + len;
				while(first != last) {
					alloc.construct(s++, *first++);
				}
				_size += len;
			}
		}
			/*initializer list*/
		basic_string& append (std::initializer_list<charT> il) {
			*this = il;
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
			if(len >= capacity) {
				iterator newstart = reallocate(2 * len + 1);
				free();	
				elements = newstart;

			}
		}
			/*substring*/
		basic_string& insert (size_type pos, const basic_string& str,
							size_type subpos, size_type sublen);
			/*c-string*/
		basic_string& insert (size_type pos, const charT* s);
			/*buffer*/
		basic_string& insert (size_type pos, const charT* s, size_type n);
			/*fill*/
		basic_string& insert (size_type pos,   size_type n, charT c);
		iterator insert (const_iterator p, size_type n, charT c);
			/*single character*/
		iterator insert (const_iterator p, charT c);
			/*range*/
		template <class InputIterator>
		iterator insert (iterator p, InputIterator first, InputIterator last);
			/*initializer list*/
		basic_string& insert (const_iterator p, std::initializer_list<charT> il);

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
		basic_string substr(size_type pos = 0, size_type len = npos) {
			basic_string ret(*this, pos, len);
		}
			/*string*/
		int compare (const basic_string& str) const noexcept;
			/*substrings*/
		int compare (size_type pos, size_type len, const basic_string& str) const;
		int compare (size_type pos, size_type len, const basic_string& str,
					size_type subpos, size_type sublen) const;
			/*c-string*/
		int compare (const charT* s) const;
		int compare (size_type pos, size_type len, const charT* s) const;
			/*buffer*/
		int compare (size_type pos, size_type len, const charT* s, size_type n) const;
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