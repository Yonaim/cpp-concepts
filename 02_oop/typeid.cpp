// https://en.cppreference.com/w/cpp/language/typeid.html
// https://en.cppreference.com/w/cpp/types/type_info.html

/*
    typeid 연산자 : 다형성 객체의 동적 유형 판별 / 정적 유형 식별을 위해 사용

    (1) typeid ( type )         : 타입명을 넘김  ex) typeid(int);
    (2) typeid ( expression )	: 표현식을 넘김  ex) typeid(*ptr);

    - 둘다 const std::type_info&를 반환
    - (1)의 경우 무조건 컴파일 타임 확정
    - (2)의 경우 기본적으로는 컴파일 타임 확정이지만, 표현식이 다형 타입의 gvalue인 경우 런타임 확인
        - 정적 타입 보는 경우: 비-다형 타입, 다형 타입이지만 gvalue가 아닌 경우
        - 동적 타입 보는 경우: 표현식이 다형 타입의 gvalue (ex. Base& / Base*를 역참조한 객체)
*/

/*
    typeid 연산자 구현

    - 컴파일러가 타입 별로 type_info 메타데이터를 바이너리에 포함시킴
    - vtable 내부 혹은 앞쪽에 type_info 데이터의 포인터가 붙어있음
    - typeid 연산자가 하는 일:
        1) p가 가리키는 객체에서 vptr 읽고
        2) vtable에서 type_info* 위치 찾아가서
        3) 그걸 std::type_info&로 뱉음

    - 결국 이 연산은 공짜가 아니고 '런타임 메모리 접근 연산 + 바이너리 크기 증가'라는 비용 존재
    - 런타임에 타입을 식별하는 것을 RTTI (Run-Time Type Information)이라고 부름
    - 컴파일 옵션 `-fno-rtti`를 붙이면 RTTI 기능을 끌 수 있다.
        - 기능을 끄면 'error: cannot use ‘typeid’ with ‘-fno-rtti’'라면서 컴파일 안됨
*/

#include <iostream>
#include <typeinfo>

struct Base
{
    virtual ~Base() {}
};
struct Derived : Base
{
};
struct Plain
{
};

// expression만 예시 코드 작성
int main()
{
    Base *pb = new Derived;
    std::cout << "typeid(*pb): " << typeid(*pb).name() << "\n"; // 동적 타입 (Derived)

    // slicing: Derived 객체를 Base 값 객체로 복사/대입할 때, Derived 클래스에만 있는 부분이
    // 잘려나감
    Base b = Derived();
    std::cout << "typeid(b):   " << typeid(b).name() << "\n"; // 정적 타입 (Base)

    Plain p;
    std::cout << "typeid(p):   " << typeid(p).name() << "\n"; // 정적 타입 (Plain)

    delete pb;
}
