//一个模板类　basic_string
//string的基础
#ifndef BASIC_STRING_H
#define BASIC_STRING_H

#include "char_traits.h"
#include <memory>  //std::　not mystl


namespace mystl {

	/*初始每次分配100，之后都是2 * 100 + 1*/
	template <typename charT, typename traits = char_traits<charT>, typename Alloc = std::allocator<charT>
	class basic_string {
	public:
		typedef traits type_traits;
		typedef Alloc allocator_type;
		typedef typename traits_type::char_type value_type;
		typedef value_type& reference;
		typedef const_value& const_reference;
		typedef typename std::allocator_traits<allocator_type>::pointer pointer;
		typedef typename std::allocator_traits<allocator_type>::const_pointer pointer;
	   	typedef charT* iterator;
		typedef const charT* const_iterator;
		typedef typename std::allocator_traits<allocator_type>::difference_type difference_t;
		typedef typename allocator_traits<allocator_type>::size_type;	
	public:
		static constexpr size_type npos = static_cast<size_type>(-1);
	private:
		allocator_type stralloc;
		iterator elements;  //开始的位置
		size_type _size;    //现有
		size_type _cap;     //cap

	public:
	/*构造函数*/
		explicit basic_string(const allocator_type &alloc = allocator_type()): stralloc(alloc), elements(stdalloc.allocate(100), _size(0), _cap(100)) { } 

		basic_string(const basic_string &str) {
			stralloc.allocate(str.cap);
		}

		basic_string(const basic_string &str, const allocator_type& alloc);
		basic_string(const basic_string &str, size_type pos, size_type len = npos,
						const allocator_type &alloc = allocator_type());
		basic_string(const charT *s, const allocator_type& alloc = allocator_type());
		basic_string(const charT *s, size_type n, const allocator_type& alloc = allocator_type());
		basic_string(size_type n, charT c, const allocator_type &alloc = allocator_type());
		template <typename InputIterator>
		basic_string(InputIterator first, InputIterator last, const allocator_type &alloc = allocator_type());

		basic_string(std::initializer_list<charT> il, const allocator_type& alloc = allocator_type());

		basic_string(basic_string &&str) noexcept;
		basic_string(basic_string &&str, const allocator_type &alloc);

		~basic_string();

		basic_string& operator=(const basic_string& str);
		basic_string& operator=(const charT *s);
		basic_string& operator=(charT c);
		basic_string& operator=(initializer_list<charT> il);
		basic_string& operator=(basic_string &&str)noexcept;
	
	/*返回迭代器*/
	public:
		iterator begin() noexcept { return elements; }
		const_iterator begin() const noexcept { return elements;}
		const_iterator cbegin() const noexcept { return elements; }
		
		iterator end() noexcept { return elements + _size; }
		const_iterator end() const noexcept{ return elements + _size;}
		const_iterator cend() const noexcept {return elements + _size; }

		size_type size() const noexcept { return _size;}
		size_type length() const noexcept { return _size;}
		size_type max_size() const noexcept { return _cap;}
		void resize(size_type n, charT c = charT());


}

#endif
