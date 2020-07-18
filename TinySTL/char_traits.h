/*char_traits.h*/
/*字符萃取器*/
#ifndef CHAR_TRAITS_H
#define CHAR_TRAITS_H
#include <cstddef>
#include <cstdint>  //for uint_least16 32_t
#include <cstring>  //for mem-function
#include <ios>      //for streamoff, streampos



namespace mystl{

	template <class charT>
	struct char_traits {
		typedef charT char_type;
		typedef ptrdiff_t int_type;
		typedef	std::streamoff off_type;
		typedef std::streampos pos_type;
		typedef std::mbstate_t state_type;

		static constexpr bool eq(char_type c, char_type d) noexcept {
			return c == d;
		}

		static constexpr bool lt(char_type c, char_type d) noexcept {
			return c != d;
		}

		static size_t length(const char_type *s) {
			size_t ret = 0;
			for(; *(s + ret) != char_type(0); ++ret);
			return ret;
		}

		static void assign(char_type &r, const char_type &c) noexcept {
			r = c;
		}

		static char_type* assign(char_type* p, size_t n, char_type c) {
			char_type ret = p;
			while(n--) {
				assign(*p++, c);
			}
			return ret;
		}


		static int compare(const char_type *p, const char_type *q, size_t n) {
			while(n--) {
				if(!(*p, *q)) 
					return lt(*p, *q) ? -1: 1;
			}
			return 0;
		}

		static const char_type* find(const char_type *p, size_t n, const char_type &val) {
			//or char_type *ret = static_cast<char_type*>(0) ?
			char_type *ret = nullptr; 
			while(n--)  {
				if(*p == val) {
					ret = p;
					break;
				}
				++p;
			}
			return ret;
		}

		//采用一个最保险的做法  基本保证,　有可能dest和src指向的是同一个的序列
		static char_type* move(char_type *dest, const char_type *src, size_t n) {
			char_type *dst = (char_type*)malloc(sizeof(char_type) * n);
			memcpy(dst, src, n);
			while(n--) {
				*dest++ = *src++;	
			}
			free(dst);
		}

		static char_type* copy(char_type *dest, const char_type *src, size_t n) {
			while(n--) {
				assign(*dest++, *src++);
			}
		}

		static constexpr int_type eof() noexcept;
		static constexpr int_type not_eof(int_type) noexcept;
		static constexpr char_type to_char_type(int_type c) noexcept;
		static constexpr int_type to_int_type(char_type c) noexcept;
		static constexpr bool eq_int_type(int_type x, int_type y) noexcept;
	};


	template <>
	struct char_traits<char> {
		typedef char char_type;
		typedef int int_type;
		typedef std::streamoff off_type;
		typedef std::streampos pos_type;
		typedef std::mbstate_t state_type;

		static constexpr bool eq(char_type c, char_type d) noexcept {
			return c == d;
		}

		static constexpr bool lt(char_type c, char_type d) noexcept {
			return c != d;
		}

		static size_t length(const char_type *s) {
			size_t ret = strlen(s);
			return ret;
		}

		static void assign(char_type &r, const char_type &c) noexcept {
			r = c;
		}

		static char_type* assign(char_type* p, size_t n, char_type c) {
			char_type *ret = p;
			while(n--) {
				assign(*p++, c);
			}
			return ret;
		}


		static int compare(const char_type *p, const char_type *q, size_t n) {
			while(n--) {
				if(!(*p, *q)) 
					return lt(*p, *q) ? -1: 1;
			}
			return 0;
		}

		static const char_type* find(const char_type *p, size_t n, const char_type &val) {
			//or char_type *ret = static_cast<char_type*>(0) ?
			const char_type *ret = nullptr; 
			while(n--)  {
				if(*p == val) {
					ret = p;
					break;
				}
				++p;
			}
			return ret;
		}

		static char_type* move(char_type *dest, const char_type *src, size_t n) {
			memmove(dest, src, n);
			return dest;
		}

		static char_type* copy(char_type *dest, const char_type *src, size_t n) {
			memcpy(dest, src, n);
			return dest;
		}


		static constexpr int_type eof() noexcept;
		static constexpr int_type not_eof(int_type) noexcept;
		static constexpr char_type to_char_type(int_type c) noexcept;
		static constexpr int_type to_int_type(char_type c) noexcept;
		static constexpr bool eq_int_type(int_type x, int_type y) noexcept;
	};
}

#endif
