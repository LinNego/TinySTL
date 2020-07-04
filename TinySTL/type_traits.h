/*类型traits*/
//是为了查看类型是否有不平凡(non-trivial)默认构造函数，拷贝构造函数，拷贝运算符，析构函数
#ifndef TYPE_TRAITS
#define TYPE_TRAITS

namespace mystl {
struct __true_type { };
struct __false_type { };

template <typename type>
    struct __type_traits {
    	typedef __true_type this_dummy_member_must_be_first; //
    
    	typedef __false_type has_trivial_default_constructor;
    	typedef __false_type has_trivial_copy_constructor;
    	typedef __false_type has_trivial_assignment_operator;
    	typedef __false_type has_trivial_destructor;
    	typedef __false_type is_POD_type;
    	
    };
    
    template <> struct __type_traits<char> {
    	typedef __true_type has_trivial_default_constructor;
    	typedef __true_type has_trivial_copy_constructor;
    	typedef __true_type has_trivial_assignment_operator;
    	typedef __true_type has_trivial_destructor;
    	typedef __true_type is_POD_type;
    };
    
    template <> struct __type_traits<signed char> {
    	typedef __true_type has_trivial_default_constructor;
    	typedef __true_type has_trivial_copy_constructor;
    	typedef __true_type has_trivial_assignment_operator;
    	typedef __true_type has_trivial_destructor;
    	typedef __true_type is_POD_type;
    };
    
    
    template <> struct __type_traits<unsigned char> {
    	typedef __true_type has_trivial_default_constructor;
    	typedef __true_type has_trivial_copy_constructor;
    	typedef __true_type has_trivial_assignment_operator;
    	typedef __true_type has_trivial_destructor;
    	typedef __true_type is_POD_type;
    };
    
    template <> struct __type_traits<short> {
    	typedef __true_type has_trivial_default_constructor;
    	typedef __true_type has_trivial_copy_constructor;
    	typedef __true_type has_trivial_assignment_operator;
    	typedef __true_type has_trivial_destructor;
    	typedef __true_type is_POD_type;
    };
    
    template <> struct __type_traits<unsigned short> {
    	typedef __true_type has_trivial_default_constructor;
    	typedef __true_type has_trivial_copy_constructor;
    	typedef __true_type has_trivial_assignment_operator;
    	typedef __true_type has_trivial_destructor;
    	typedef __true_type is_POD_type;	
    };
    
    template <> struct __type_traits<int> {
    	typedef __true_type has_trivial_default_constructor;
    	typedef __true_type has_trivial_copy_constructor;
    	typedef __true_type has_trivial_assignment_operator;
    	typedef __true_type has_trivial_destructor;
    	typedef __true_type is_POD_type;
    	
    };
    
    template <> struct __type_traits<unsigned int> {
    	typedef __true_type has_trivial_default_constructor;
    	typedef __true_type has_trivial_copy_constructor;
    	typedef __true_type has_trivial_assignment_operator;
    	typedef __true_type has_trivial_destructor;
    	typedef __true_type is_POD_type;
    };
    
    template <> struct __type_traits<long> {
    	typedef __true_type has_trivial_default_constructor;
    	typedef __true_type has_trivial_copy_constructor;
    	typedef __true_type has_trivial_assignment_operator;
    	typedef __true_type has_trivial_destructor;
    	typedef __true_type is_POD_type;
    };
    
    template <> struct __type_traits<unsigned long> {
    	typedef __true_type has_trivial_default_constructor;
    	typedef __true_type has_trivial_copy_constructor;
    	typedef __true_type has_trivial_assignment_operator;
    	typedef __true_type has_trivial_destructor;
    	typedef __true_type is_POD_type;
    };
    
    template <> struct __type_traits<long long> {
    	typedef __true_type has_trivial_default_constructor;
    	typedef __true_type has_trivial_copy_constructor;
    	typedef __true_type has_trivial_assignment_operator;
    	typedef __true_type has_trivial_destructor;
    	typedef __true_type is_POD_type;
    };
    
    template <> struct __type_traits<float> {
    	typedef __true_type has_trivial_default_constructor;
    	typedef __true_type has_trivial_copy_constructor;
    	typedef __true_type has_trivial_assignment_operator;
    	typedef __true_type has_trivial_destructor;
    	typedef __true_type is_POD_type;
    };
    
    template <> struct __type_traits<double> {
    	typedef __true_type has_trivial_default_constructor;
    	typedef __true_type has_trivial_copy_constructor;
    	typedef __true_type has_trivial_assignment_operator;
    	typedef __true_type has_trivial_destructor;
    	typedef __true_type is_POD_type;
    };
    
    template <> struct __type_traits<long double> {	
    	typedef __true_type has_trivial_default_constructor;
    	typedef __true_type has_trivial_copy_constructor;
    	typedef __true_type has_trivial_assignment_operator;
    	typedef __true_type has_trivial_destructor;
    	typedef __true_type is_POD_type;
    };
    
    template <typename T> struct __type_traits<T*> {
    	typedef __true_type has_trivial_default_constructor;
    	typedef __true_type has_trivial_copy_constructor;
    	typedef __true_type has_trivial_assignment_operator;
    	typedef __true_type has_trivial_destructor;
    	typedef __true_type is_POD_type;
    }; //不需要const T*的偏特例化,可以直接使用T*的偏特化。
}
#endif
