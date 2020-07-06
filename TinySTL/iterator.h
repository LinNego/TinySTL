//STL源码剖析
/*模板元编程的重要组成一环，通过迭代器获取迭代器所指的类型*/
/*设计这个最重要的的原因在于？迭代器：行为像指针的类*/
#ifndef MYSTL_ITERATOR_H
#define MYSTL_ITERATOR_H
#include <cstddef>

namespace mystl {

    struct input_iterator_tag {};
    struct output_iterator_tag {};
    struct forward_iterator_tag: public input_iterator_tag {};
    struct bidirectional_iterator_tag: public forward_iterator_tag {};
    struct  random_access_iterator_tag: public bidirectional_iterator_tag {};
    
    template <typename Category, typename T, typename Distance = ptrdiff_t, typename Pointer = T*, typename Reference = T&>
    struct iterator {
    	typedef Category iterator_category;
    	typedef T value_type;
    	typedef Distance difference_type;
    	typedef Pointer pointer;
    	typedef Reference reference;	
    };
    
    template <typename Iterator>
    struct iterator_traits {
    	typedef typename Iterator::iterator_category iterator_category;
    	typedef typename Iterator::value_type value_type;
    	typedef typename Iterator::difference_type difference_type;
    	typedef typename Iterator::pointer pointer;
    	typedef typename Iterator::reference reference;
    };
    /*理解偏特化的含义*/
    template <typename T>
    struct iterator_traits<T*> {
    	typedef T value_type;
    	typedef T* pointer;
    	typedef T& reference;
    	typedef random_access_iterator_tag iterator_category;
    	typedef ptrdiff_t differenece_type;
    };
    
    template <typename T>
    struct iterator_traits<const T*> {
    	typedef T value_type;
    	typedef ptrdiff_t dieffence_type;
    	typedef const T* pointer;
    	typedef const T& reference;
    	typedef random_access_iterator_tag iterator_category;
    };
    
    template <typename Iterator>
    inline typename iterator_traits<Iterator>::iterator_category
    iterator_category(const Iterator&) {
    	typedef typename iterator_traits<Iterator>::iterator_category category;
    	return category();
    }
    
    template <typename Iterator>
    inline typename iterator_traits<Iterator>::difference_type*  //为什么返回的是一个指针
    distance_type(const Iterator&) {
    	return static_cast<typename iterator_traits<Iterator>::difference_type*>(nullptr);
    }
    
    template <typename Iterator>
    inline typename iterator_traits<Iterator>::value_type
    value_type(const Iterator&) {
    	return static_cast<typename iterator_traits<Iterator>::value_type*>(nullptr);
		//typedef typename iterator_traits<Iterator>::value_type value_type;
		//return value_type();
    }
    
    template <typename InputIterator>
    inline typename iterator_traits<InputIterator>::difference_type
    __distance(InputIterator first, InputIterator last, input_iterator_tag) {
    	typename iterator_traits<InputIterator>::diffence_type n = 0;
    	while(first != last) {
    		++n;
    		++first;	
    	}
    	return n;
    }
    
    template <typename RandomAccessIterator>
    inline typename iterator_traits<RandomAccessIterator>::difference_type
    __distance(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag) {
    	return last - first;
    }
    
    template <typename InputIterator>
    inline typename iterator_traits<InputIterator>::difference_type
    distance(InputIterator first, InputIterator last) {
    	typedef typename
    			iterator_traits<InputIterator>::iterator_category category;
    	return __distance(first, last, category());
    }
    
    
    template <typename InputIterator, typename Distance>
    inline void __advance(InputIterator &i, Distance n, input_iterator_tag) {
    	while(n--) ++i;
    }
    
    template <typename BidirectionalIterator, typename Distance>
    inline void __advance(BidirectionalIterator &first, Distance n, bidirectional_iterator_tag) {
    	if(n >= 0) {
    		while(n--) ++first;
    	}
    	else {
    		while(n++) --first;
    	}
    }
    
    template <typename RandomAccessIterator, typename Distance>
    inline void __advance(RandomAccessIterator &i, Distance n, random_access_iterator_tag) {
    	i += n;   //似乎没有出错处理
    }
    
    
    template <typename InputIterator, typename Distance>
    inline void advance(InputIterator &first, Distance n) {
    	typedef typename iterator_traits<InputIterator>::iterator_category category;
    	return __advance(first, n, category());
    }
}
#endif

