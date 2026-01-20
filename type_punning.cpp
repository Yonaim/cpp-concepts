#include <iostream>

/*
    Type Punning: '같은 메모리 바이트를 다른 타입으로 해석해서' 값을 읽거나 쓰는 테크닉
        (punning: 말장난)
*/

struct Base
{
    int b = 10;
    virtual ~Base() {} // 다형성 타입
};

struct Derived : Base
{
    // Base 타입의 메모리 레이아웃 그대로 가짐
    int d = 20;
};

int main()
{
    Base  base; // 진짜 객체는 Base
    Base *pb = &base;
    // UB: 실제 객체가 Derived가 아닌데 Derived로 취급
    Derived *pd = reinterpret_cast<Derived *>(pb);

    // pd->d는 Base 객체의 "존재하지 않는 영역"을 읽는 꼴
    std::cout << pd->d << "\n"; // UB
}
