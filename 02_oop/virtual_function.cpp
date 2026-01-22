#include <iostream>

/*
    Virtual Function (가상 함수) 컨셉 정리

    1) 목적: 동적 바인딩(dynamic dispatch)
       - 포인터/레퍼런스의 "정적 타입"이 Base여도,
         실제 객체의 "동적 타입"이 Derived이면 Derived 함수가 호출되게 한다.
       - 즉, 런타임에 "진짜 객체 타입"을 기준으로 함수가 결정된다. (dynamic dispatch)

    2) virtual이 없으면?
       - 정적 바인딩(static binding)
       - Base* / Base& 타입으로 호출하면 항상 Base 버전이 호출된다.
         (객체가 Derived여도 정적 타입이 Base면 Base 함수로 고정)

    3) override 키워드
       - 실제로 오버라이드가 맞는지 컴파일 타임에 체크해주는 안전장치
       - 시그니처가 조금이라도 다르면(예: const 빠짐, 매개변수 타입 다름) 컴파일 에러로 잡아줌

    4) 소멸자는 무조건 virtual 함수
       - Base 포인터로 Derived 객체를 가리킨다면 소멸할 때는 ~Derived()가 호출되어야 함
        - 따라서 소멸자는 무조건 virtual로 선언
*/

/*
    - 클래스 내부의 함수가 하나라도 가상 함수라면 vtable이 생성되고,
        해당 타입의 메모리 레이아웃 맨 앞에 vtable의 포인터가 삽입됨
    - 가상 함수를 하나라도 가지면 polymorphic class (다형 클래스)

    - Base 클래스가 polmorhpic class라면 Derived 클래스도 dynamic dispatch 수행
        - 세부 구현 따라 다를 수 있지만 보통은 Derived 클래스도 vtable을 갖게 됨
        - Derived 클래스의 vtable: 값이 복사된 형태 (base 클래스의 vtable을 동적 접근하지 않음)

    - 가상 함수 선언을 원하면, 반드시 base 클래스에 virtual 키워드를 사용
        - Derived 클래스에서 virtual 키워드를 사용하는 건 자유 (단순한 표기 중복)
        - Derived 클래스에선 virtual보단 override 키워드를 사용하는 것을 권장
*/

struct Base
{
    virtual ~Base() { std::cout << "~Base\n"; }

    virtual void speak() { std::cout << "Base::speak\n"; }

    void hello() { std::cout << "Base::hello (non-virtual)\n"; }
};

struct Derived : Base
{
    ~Derived() { std::cout << "~Derived\n"; }

    void speak() override { std::cout << "Derived::speak\n"; }

    void hello() { std::cout << "Derived::hello (hides Base::hello)\n"; }
};

int main()
{
    Derived d;

    std::cout << "\n--- Direct call (object) ---\n";
    d.speak();
    d.hello();

    std::cout << "\n--- Static type = Base* (polymorphism) ---\n";
    Base *p = &d;

    p->speak(); // virtual: dynamic dispatch
    p->hello(); // non-virtual: static binding

    std::cout << "\n--- Deleting through Base* ---\n";
    Base *heap = new Derived();
    delete heap; // ~Derived then ~Base
}
