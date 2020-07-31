//LinNego
//test_string.h
#include "../TinySTL/string.h"
#include <iostream>
#include <memory>
#include <initializer_list>
#include <string>
using std::cin;
using std::cout;
using std::endl;
using std::allocator;
using std::initializer_list;
using namespace mystl;
int main() {
    string a;
    string b("hello world");
    string c = b;
    allocator<char> all;
    string d(c, all);
    string e(c, 0);
    string f("hello world", 11);
    string g(11, 'c');
    string h(b.begin(), b.end());
    string i(std::move(string(h)));
   // string i(string(h));    //一个右值为什么没有调用移动构造函数呢？需要显示std::move()
    string j({'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r','l', 'd'});
    /*
    cout << b << endl;
    cout << c << endl;
    cout << d << endl;
    cout << e << endl;
    cout << f << endl;
    cout << g << endl;
    cout << h << endl;
    cout << i << endl;
    cout << j << endl;
    a = b;
    cout << a << endl;
    a = "hello world";
    cout << a << endl;
    a = 'c';
    cout << a << endl;  //cello world
    a = {'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
    cout << a << endl;



    //测试成员函数
    cout << *a.begin() << endl;
   // cout << *a.end() << endl;   //行为未定义
    cout << a.size() << endl;
    cout << a.length() << endl;
    cout << a.max_size() << endl;
    a.resize(10);
    cout << a << endl;
    //a.resize(200);
    //cout << a << endl;
    a.resize(200, 'd');
    cout << a << endl;
    a.resize(11);
    cout << a << endl;
    cout << a.capacity() << endl;  //401
    a.reserver(500);
    cout << a.capacity() << endl;  //1001

    a.clear();
    cout << a << endl;
    cout << b[0] << endl;
    
    cout << b[20] << endl; //超出_size()行为未定义
    cout << b.at(0) << endl;
    cout << b.back() << endl; //如果是空，行为未定义
    cout << b.front() << endl;//如果为空，行为未定义

    b += c;
    cout << b << endl;

    b += "hello world";
    cout << b << endl;

    b += 'h';
    cout << b << endl;

    b += {'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
    cout << b << endl;

    b.append(c);
    cout << b << endl;

    b.append(c, 0, 1);
    cout << b << endl;

    b.append("ello world");
    cout << b << endl;

    b.append("hello world trash", 11);
    cout << b << endl;

    b.append(5, 't');
    cout << b << endl;

    b.append(c.begin(), c.end()); 
    cout << b << endl;

    b.append({'h', 'e', 'l', 'l', 'o' ,' ', 'w', 'o', 'r', 'l' , 'd'});
    cout << b << endl;

    b.push_back('h');
    cout << b << endl;

    b.assign(c);
    cout << b << endl;

    b.assign(c, 1, 4);
    cout << b << endl;

    b.assign("hello world");
    cout << b << endl;

    b.assign("hello world", 5);
    cout << b << endl;

    b.assign(10, 'c');
    cout << b << endl;

    b.assign(c.begin(), c.end());
    cout << b << endl;

    b.assign({'t','e','s','t'});
    cout << b << endl;

    b = c;
    b.insert(1, c);  //pos >= size() 抛出out_of_range异常
    cout << b << endl;

    b.insert(1, c, 0, 2);
    cout << b << endl;

    //b.insert(100, 10, 'c');
    //cout << b << endl;

  //  b.insert(c.begin(), 'c'); //如果不是迭代器没有指向本区域，抛出out_of_range()
    b.clear();
    b.insert(b.begin(), c.begin(), c.end());
    cout << b << endl;

    b.insert(b.begin(), {'h'});
    cout << b << endl;

    b = "hello world";
    b.erase(0, 2);
    cout << b << endl;

    b.erase(b.begin());
    cout << b << endl;

    b.erase(b.begin(), b.end());
    cout << b << endl;

    b = "hello world";
    b.replace(0, 1, c);
    cout << b << endl;
    */
    b = "";
    b.replace(b.begin(), b.end(), c);
    cout << b << endl;

    b.replace(0, 5, "Hello");
    cout << b << endl;
    b.replace(0, 5, "hello >>>>", 7);
    cout << b << endl;

    b.replace(0, b.size(), 7, 'h');
    cout << b << endl;

    b.replace(b.begin(), b.end(), "hello world");
    cout << b << endl;
    b = "";
    b.replace(b.begin(), b.end(), c.begin(), c.end());
    cout << b << endl;
    b = "wo";
    b.swap(c);
    cout << b << endl;
    cout << c << endl;
    b.pop_back();
    cout << b << endl;


    b = "hello world hello wolrd";
    auto t = b.find(string("world"));
    cout << t << endl;

    //t = b.find("wolrd");
    b = "hello world";
    c = "hello world";
    cout << c << endl;
    cout << b.compare(c) << endl;

    cout << b.compare(0, 5, c) << endl;
    cout << b.compare(0, 5, c, 0, 5) << endl;

    cout << b.compare("hello") << endl;
    cout << b.compare(0, 5, "hello") << endl;
    
    cout << b.compare(0, 5, "hello", 5) << endl;

    cout << b << " " << c << endl;
    //b += c;
    //cout << b << endl;
    b = b + c;
    cout << b << endl;
    //cout << t << endl;
    std::string s;
    //s.insert(10, "hello");
    //cout << s << endl;
    return 0;
}