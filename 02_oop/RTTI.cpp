#include <iostream>
#include <typeinfo>

/*
    RTTI (Run-Time Type Information) 컨셉 정리

    [RTTI란?]
    - 런타임에 "이 포인터/레퍼런스가 실제로 가리키는(동적 타입) 타입"을 알아내는 메커니즘
    - 대표적으로 RTTI를 사용하는 기능: dynamic_cast, typeid

    [RTTI가 동작하는 조건]
    -'다형 클래스(polymorphic class)'에서 RTTI가 의미 있게 동작한다.
        => 가상 함수(virtual)가 1개라도 있는 타입
    - 이유: vtable/vptr 같은 런타임 메타데이터를 통해 '실제 타입'을 식별할 수 있기 때문.

    [대표적인 RTTI]
    1) dynamic_cast
        - 안전한 다운캐스트/크로스캐스트를 제공
        - RTTI 메타데이터(= typeinfo 계열)에 접근해서
            “이 포인터가 진짜 그 타입으로 안전하게 변환 가능한지”를 검사

    2) typeid
       - typeid(expr)는 expr의 타입 정보를 std::type_info로 반환
       - polymorphic 타입에서, typeid(*base_ptr)은 "동적 타입" 기준으로 나온다.
       - 비-polymorphic 타입이면, typeid는 사실상 "정적 타입" 정보만 준다.

    [RTTI의 비용]
    - RTTI는 표준에서 제공하는 기능이지만 비용/설계를 고려해야 한다
    - 가능하면 런타임에 RTTI를 여러 번 써서 스위치/분기하는 대신,
        가상 함수 호출을 이용해 타입이 알아서 처리하도록 하기
*/

struct Base
{
    virtual ~Base() { std::cout << "~Base\n"; } // polymorphic을 만들기 위한 대표적 패턴

    virtual void speak() { std::cout << "Base::speak\n"; }
};

struct Derived : Base
{
    ~Derived() { std::cout << "~Derived\n"; }

    void speak() override { std::cout << "Derived::speak\n"; }

    void only_derived() { std::cout << "Derived::only_derived\n"; }
};

struct Another : Base
{
    ~Another() { std::cout << "~Another\n"; }

    void speak() override { std::cout << "Another::speak\n"; }
};

static void print_type(const Base *p)
{
    std::cout << "  [typeid(*p).name()] = " << typeid(*p).name() << "\n";
}

int main()
{
    std::cout << "\n--- 1) dynamic_cast pointer: 성공/실패 ---\n";
    {
        Base *p1 = new Derived();
        Base *p2 = new Another();

        if (Derived *d1 = dynamic_cast<Derived *>(p1))
        {
            std::cout << "p1 -> dynamic_cast<Derived*> success\n";
            d1->only_derived();
        }
        else
        {
            std::cout << "p1 -> dynamic_cast<Derived*> failed\n";
        }

        if (Derived *d2 = dynamic_cast<Derived *>(p2))
        {
            std::cout << "p2 -> dynamic_cast<Derived*> success (unexpected)\n";
            d2->only_derived();
        }
        else
        {
            std::cout << "p2 -> dynamic_cast<Derived*> failed (nullptr)\n";
        }

        delete p1;
        delete p2;
    }

    std::cout << "\n--- 2) dynamic_cast reference: 실패 시 bad_cast ---\n";
    {
        Another a;
        Base   &br = a;

        try
        {
            Derived &dr = dynamic_cast<Derived &>(br); // 실제 객체는 Another -> 실패
            (void)dr;
            std::cout << "dynamic_cast<Derived&> success (unexpected)\n";
        }
        catch (const std::bad_cast &e)
        {
            std::cout << "dynamic_cast<Derived&> failed: std::bad_cast\n";
            std::cout << "  what(): " << e.what() << "\n";
        }
    }

    std::cout << "\n--- 3) typeid: polymorphic에서 동적 타입 확인 ---\n";
    {
        Derived d;
        Base   *p = &d;

        std::cout << "typeid(d).name():    " << typeid(d).name() << "\n";
        std::cout << "typeid(*p).name():   " << typeid(*p).name() << "  (dynamic type)\n";
        std::cout << "typeid(p).name():    " << typeid(p).name() << "  (pointer type)\n";
    }

    std::cout << "\n--- 4) typeid + 분기 예시 (RTTI 기반 타입 분기) ---\n";
    {
        Base *arr[3];
        arr[0] = new Base();
        arr[1] = new Derived();
        arr[2] = new Another();

        for (int i = 0; i < 3; ++i)
        {
            Base *p = arr[i];

            std::cout << "obj[" << i << "]:\n";
            print_type(p);

            // "정확히 Derived인가?" 같은 분기 (가끔 필요하지만, 과하면 설계 냄새)
            if (typeid(*p) == typeid(Derived))
                std::cout << "  -> exact type is Derived\n";
            else if (typeid(*p) == typeid(Another))
                std::cout << "  -> exact type is Another\n";
            else if (typeid(*p) == typeid(Base))
                std::cout << "  -> exact type is Base\n";

            p->speak();
        }

        for (int i = 0; i < 3; ++i)
            delete arr[i];
    }

    std::cout << "\n--- 5) typeid(*p) with nullptr (주의) ---\n";
    {
        Base *p = nullptr;

        try
        {
            // Base가 polymorphic이면, typeid(*p)는 nullptr 역참조를 감지하고 std::bad_typeid를 던질
            // 수 있음
            std::cout << "typeid(*p).name(): " << typeid(*p).name() << "\n";
        }
        catch (const std::bad_typeid &e)
        {
            std::cout << "typeid(*p) failed: std::bad_typeid\n";
            std::cout << "  what(): " << e.what() << "\n";
        }
    }
}
