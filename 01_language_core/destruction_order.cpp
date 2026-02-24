/*
    소멸(destruction) 순서 : 객체가 "언제/어떤 순서로" 파괴되는지 확인하는 예제

    (1) static storage duration (static, 정적 변수)
        - 프로그램 종료 시 소멸
        - (같은 TU 내에서는) 생성의 역순
        - 다른 TU인 경우 순서 보장되지 않음

    (2) auto storage duration (auto, 지역 변수)
        - 스코프를 벗어날 때 소멸
        - 생성의 역순으로 소멸

    (3) 클래스 (base 클래스, 내부 멤버)
        - 생성:  Base -> Member(선언 순서) -> Derived
        - 소멸:  Derived 본문 -> Member(선언 역순) -> Base

    (4) 임시 객체(temporary)
        - 일반적으로 "전체 표현식(full-expression)"이 끝날 때 소멸
        - 예: `Make().Method();`에서 Make()가 만든 임시는 그 문장 끝에서 소멸

*/

#include <iostream>
#include <string>

struct Trace
{
    std::string Name;

    Trace(const char *n) : Name(n) { std::cout << "[ctor] " << Name << "\n"; }

    ~Trace() { std::cout << "[dtor] " << Name << "\n"; }
};

struct Base
{
    Trace BaseMem{"Base::BaseMem"};

    Base() { std::cout << "  Base()\n"; }

    virtual ~Base() { std::cout << "  ~Base()\n"; }
};

struct Derived : Base
{
    // 멤버는 "선언 순서대로" 생성되고, "선언 역순으로" 소멸됨
    Trace A{"Derived::First"};
    Trace B{"Derived::Second"};

    Derived() { std::cout << "  Derived()\n"; }

    ~Derived() override { std::cout << "  ~Derived()\n"; }
};

static Trace GStatic{"[static] GStatic"}; // 프로그램 종료 시 소멸

Base MakeBase()
{
    // local 생성 -> 반환값 생성 -> local 소멸
    Trace local{"Local"};
    std::cout << "  MakeBase() returning Base\n";
    return Base{};
}

int main()
{
    std::cout << "\n=== (1) static storage duration (static, 정적 변수) ===\n\n";


    std::cout << "\n=== (2) auto storage duration (auto, 지역 변수) ===\n\n";
    // 생성: x -> y
    // 소멸: y -> x
    {
        Trace x{"block::x"};
        Trace y{"block::y"};
        std::cout << "\n  (leaving block)\n\n";
    }

    std::cout << "\n=== (3) 클래스 (base 클래스, 내부 멤버) ===\n\n";
    // 생성: base -> derived (내부 멤버는 선언 순서를 따름)
    // 소멸: derived -> base
    {
        Derived d;
        std::cout << "\n  (leaving scope)\n\n";
    }

    std::cout << "\n=== (4) 임시 객체(temporary) ===\n\n";
    {
        std::cout << "  (full-expression begins)\n";

        // 반환된 임시 Derived는 이 표현식 끝에서 소멸 
        MakeBase(); 

        std::cout << "\n  (after MakeDerived(); statement)\n";
    }

    std::cout << "\n=== main end ===\n";
}