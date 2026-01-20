#include <iostream>


/*
    [소멸자가 예외를 던지지 말아야하는 이유]

    1) unwinding 중에 이미 예외 처리를 진행 중임. 
        그런데 그 와중에 호출되는 소멸자에서도 예외가 던져지면 예외 처리 비용/복잡도가 폭발
    2) 소멸 실패를 책임질 주체가 모호함
        소멸자는 호출이 자동인 경우가 많기 때문에 소멸 실패를 책임질 주체가 모호함
    3) 원소들을 연속적으로 소멸시키는 경우에,
        중간에 한 원소가 소멸 실패했을시 exception-safety를 보장하기가 어려움
        (즉 그 이전 원소들에 대해 소멸 이전으로 돌릴 수 있음을 보장할 수 없음)

    => C++11부터는 아예 소멸자는 기본적으로 noexcept
        만약 소멸자에서 예외가 던져질시 std::terminate()가 호출됨
    
    소멸자는 예외 전파가 아니라 정리를 위한 함수이다!!!
    - 자원 정리 중 실패 가능성이 있을 경우 소멸자가 아니라 별도의 close() 함수로 제공하여 호출자가 처리
    - 소멸자는 try-catch문 + 로깅 정도만... 절대 밖으로 전파하지 말기
*/

/*
    - noexcept 키워드
        - 이 함수는 예외를 안 던진다
        - noexcept(true)랑 똑같음
        - noexcept(false)는 안 쓴거랑 동일 (예외 던질 수도 있다)
*/

struct B
{
    B() { std::cout << __func__ << "\n"; }
    ~B()
    {
        std::cout << __func__ << "\n";
        throw std::runtime_error("Second exception"); // 예외 발생
    }
};

struct C
{
    C()
    {
        std::cout << __func__ << "\n";
        throw std::runtime_error("First exception"); // 예외 발생
    }
    ~C() { std::cout << __func__ << "\n"; }
};

void f1()
{
    std::cout << __func__ << "\n";

    B b; // fully constructed
    C c; // 생성 중 예외 발생!
    // 이제 b의 소멸자가 호출됨

    std::cout << "f1 end\n"; // 출력됨
}

void f2() { std::cout << __func__ << "\n"; }

int main()
{
    std::cout << "start\n";

    f1();

    std::cout << "main end\n"; // 출력됨
}
