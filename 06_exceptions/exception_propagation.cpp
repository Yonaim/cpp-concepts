#include <iostream>
#include <stdexcept>

/*
    예외 전파(Exception propagation)
    : throw된 예외가 콜스택을 따라 위로 올라가며 잡힐 곳을 찾는 과정
*/

/*
    stack unwinding(스택 언와인딩)
    - 예외가 위로 전파되는 동안, 예외가 지나쳐 간 함수들은 정상 종료(return)가 아니라 비정상 탈출을 함
    - 이때 해당 함수의 지역 객체들의 소멸자(destructor)가 역순으로 호출됨
    - 이걸 스택이 풀린다(unwind)고 해서 stack unwinding이라고 부름!!
        => C++에서 RAII)가 강력한 이유
*/

/*
    stack unwinding시...
    - 현재 try 블록 내부의 남은 코드는 실행 안 됨
    - 현재 함수의 남은 코드도 실행 안 됨
    - 대신, 스코프를 빠져나가며 소멸자만 실행
    - 적절한 catch에 도착하면 그 catch 블록부터 실행 재개
*/

struct Guard
{
    const char *name;
    Guard(const char *n) : name(n) { std::cout << "ctor " << name << "\n"; }
    ~Guard() { std::cout << "dtor " << name << "\n"; }
};

void f()
{
    Guard a("a");
    Guard b("b");
    throw std::runtime_error("boom"); // 예외 발생
}

int main()
{
    std::cout << "start\n";
    f(); // 여기서 catch 안 함 -> main까지 전파 -> std::terminate()
    std::cout << "unreachable\n";
}
