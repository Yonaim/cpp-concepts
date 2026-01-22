// https://jabdon4ny.tistory.com/110

/*
    std::atomic<T>: 타입 T에 대해 원자적으로 보호
    - OS의 lock (mutex등)을 사용하지 않고 구현한 경우 lock-free라고 부름
        - 컴파일러 builtin +  CPU 원자 명령으로 구현 가능
    - lock_free 여부는 is_lock_free() 멤버 함수 호출을 통해 알 수 있다
    - mutex를 사용해 구현하면 오버헤드가 커서 lock free면 좋음.
*/

/*
    lock free라고 해서 무조건 좋은 건 아니다
    - 일반적으로 8바이트 이하의 타입에서만 atomic을 지원함
    - 근데 atomic 지원이 불가한 타입에서 std::atomic을 사용하면,
      내부적으로 spin lock을 사용하게 된다.
*/

// g++ std_atomic.cpp -std=c++20

#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

std::atomic_int acnt;
int             cnt;

void f()
{
    for (auto n{10000000}; n; --n)
    {
        ++acnt;
        ++cnt;
    }
}

int main()
{
    {
        std::vector<std::jthread> pool;
        for (int n = 0; n < 10; ++n)
            pool.emplace_back(f);
    }

    std::cout << "The atomic counter is " << acnt << '\n'
              << "The non-atomic counter is " << cnt << '\n';
}
