#include <iostream>
#include <vector>

/*
    - 두 가지 생성자
    - 한 생성자는 인자 1개만, 다른 생성자는 기본 값을 갖는 인자 하나를 더 추가로 갖음
    - 오버로드 해석(overload resolution)이 성공적으로 이루어질까?
*/

struct someType
{
    someType(int arg1) { std::cout << "Constructor 1 called" << std::endl; }
    someType(int arg1, int arg2 = 42) { std::cout << "Constructor 2 called" << std::endl; }
};

struct myMap
{
    template <class InputIterator>
    myMap(InputIterator first, InputIterator last)
    {
        std::cout << "Constructor 1 called" << std::endl;
    }
    template <class InputIterator>
    myMap(InputIterator first, InputIterator last, const int alloc = 42)
    {
        std::cout << "Constructor 2 called" << std::endl;
    };
};

int main()
{
    // someType obj(10);
    myMap map(1, 2);
}
