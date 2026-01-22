#include <iostream>

/*
    stack unwinding(스택 언와인딩)
    - 예외가 위로 전파되는 동안, 예외가 지나쳐 간 함수들은 정상 종료(return)가 아니라 비정상 탈출을
   함
    - 이때 해당 함수의 지역 객체들의 소멸자(destructor)가 역순으로 호출됨
    - 이걸 스택이 풀린다(unwind)고 해서 stack unwinding이라고 부름!!
        => C++에서 RAII가 강력한 이유
*/

/*
    stack unwinding시...
    - 현재 try 블록 내부의 남은 코드는 실행 안 됨
    - 현재 함수의 남은 코드도 실행 안 됨
    - 대신, 스코프를 빠져나가며 소멸자만 실행
    - 적절한 catch에 도착하면 그 catch 블록부터 실행 재개
*/

struct A
{
    A() { std::cout << __func__ << "\n"; }
    ~A() { std::cout << __func__ << "\n"; }
};

struct B
{
    B()
    {
        std::cout << __func__ << "\n";
        throw std::runtime_error("boom"); // 예외 발생
    }
    ~B() { std::cout << __func__ << "\n"; }
};

struct C
{
    C() { std::cout << __func__ << "\n"; }
    ~C() { std::cout << __func__ << "\n"; }
};

void f1()
{
    std::cout << __func__ << "\n";
    try
    {
        A a; // 정상 생성 (fully constructed)
        B b; // 생성 중 예외 발생! (fully constructed 되지 않음)

        std::cout << "unreachable\n";
        C c; // 진행되지 않음 (fully constructed 되지 않음)
    }
    catch (const std::exception &e)
    {
        // stack unwinding: 이미 생성된 지역변수 A를 정리한다.
        // 여기서 ~A() 호출됨
        std::cerr << e.what() << '\n';
    }
    std::cout << "f1 end\n"; // 출력됨
}

void f2() { std::cout << __func__ << "\n"; }

int main()
{
    std::cout << "start\n";

    f1();

    std::cout << "main end\n"; // 출력됨
}
