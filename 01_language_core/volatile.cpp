/*
    volatile
    - "레지스터 저장 최적화를 하지 않고 매번 메모리에서 다시 읽어야 함"을 컴파일러에게 알리는 키워드
    - 외부에 의해 값이 바뀔 수 있을 때 사용하는 키워드이다.
        - 스레드 동기화를 위한 키워드가 아님!!! 외부장치와의 동기화를 위한 키워드라고 할 수 있음
        - 여기서 외부 장치는 시그널 / IO장치와 같은 것이 될 수 있음. 이 예시는 시그널
*/

/*
    g++ volatile.cpp -DUSE_VOLATILE=0 -O1
    g++ volatile.cpp -DUSE_VOLATILE=1 -O1

    -S 옵션을 통해 어셈블리 코드를 뽑아 위 두 버전을 비교해보면

    .L3:
        testl	%eax, %eax
        je	.L3

    .L3:
        movl	_ZL7g_ready(%rip), %eax
        testl	%eax, %eax
        je	.L3    

    전자는 캐싱된 값을 그대로 사용하고 후자는 g_ready를 매번 읽는 것을 알 수 있다.
*/

/*
    사용방법:
    -DUSE_VOLATILE=1 로 컴파일하면 volatile 버전
    -DUSE_VOLATILE=0 로 컴파일하면 non-volatile 버전
*/

#include <csignal>
#include <cstdio>
#include <unistd.h> // alarm()

#ifndef USE_VOLATILE
    #define USE_VOLATILE 1
#endif

#if USE_VOLATILE
static volatile sig_atomic_t g_ready = 0;
#else
static sig_atomic_t g_ready = 0;
#endif

static void on_alarm(int /*signum*/) { g_ready = 1; }

int main()
{
    std::signal(SIGALRM, on_alarm);
    alarm(1);

    std::printf("USE_VOLATILE=%d\n", USE_VOLATILE);
    std::puts("waiting... (should exit after ~1 second)");
    while (!g_ready)
    {
        // busy wait
        // volatile 버전일시: g_ready를 매 루프마다 읽는다
        // non-volatile 버전일시: g_ready의 캐싱된 값을 계속 읽는다 -> 무한루프
    }

    std::puts("done!");
    return 0;
}