#include <iostream>

/*
================================================================================
Devirtualization(가상 호출 제거)

devirtualize가 되는 조건:
- 컴파일러가 “p의 동적 타입이 항상 하나(예: DerivedA)다”를 증명/추론할 수 있으면
  => vtable을 안 거치고 DerivedA::speak로 직행(심지어 인라인 가능)

예제 구성:
- foo(Base*): 타입이 런타임에 갈릴 수 있어 보통 제거 불가
- bar_fixed(): 지역변수로 타입이 확정이라 제거 가능성이 큼(최적화 시)
- bar_final(): final로 더 강하게 막아줘서 제거 가능성이 더 커짐
================================================================================
*/

struct Base
{
    virtual ~Base() {}
    virtual void speak() { std::cout << "[virtual] Base::speak\n"; }
};

struct DerivedA : Base
{
    void speak() override { std::cout << "[virtual] DerivedA::speak\n"; }
};

struct DerivedB : Base
{
    void speak() override { std::cout << "[virtual] DerivedB::speak\n"; }
};

// (A) 제거 불가(대표): p의 동적 타입이 호출자마다 달라질 수 있음
void foo(Base *p)
{
    // 컴파일러는 "Base::speak 슬롯"을 호출한다는 건 아는데,
    // 실제 구현(override)은 p의 동적 타입에 따라 런타임에 결정됨.
    p->speak();
}

// (B) 제거 가능(대표): 타입이 사실상 확정
void bar_fixed()
{
    DerivedA d;
    Base    *p = &d;

    // 최적화(-O2, LTO 등)에서 p가 항상 DerivedA라고 확신하면
    // vtable 조회 없이 DerivedA::speak로 직행/인라인될 수 있음.
    p->speak();
}

// (C) 더 제거 쉬운 힌트: final
struct FinalA final : Base
{
    void speak() override { std::cout << "[virtual] FinalA::speak\n"; }
};

void bar_final()
{
    FinalA d;
    Base  *p = &d;

    // final은 "여기서 더 override될 수 없다"는 강한 힌트라
    // devirtualize가 더 쉬워지는 경우가 많음.
    p->speak();
}

int main()
{
    std::cout << "==================== devirtualization demo ====================\n";

    DerivedA a;
    DerivedB b;

    std::cout << "\n--- foo(Base*) : runtime type may vary ---\n";
    foo(&a);
    foo(&b);

    std::cout << "\n--- bar_fixed() : type is fixed (often devirtualizable) ---\n";
    bar_fixed();

    std::cout << "\n--- bar_final() : final makes it even easier ---\n";
    bar_final();
}
