// TCO: Tail Call Optimization

/*
tail recursion (꼬리 재귀) & tail-call optimization (TCO):
- "재귀 호출이 함수의 마지막 동작"이면 꼬리 재귀(tail recursion)이다.
  => 재귀 호출 뒤에 +, *, 비교, 후처리 등이 남아 있으면 꼬리 재귀가 아님.
- 꼬리 재귀는 컴파일러가 최적화(TCO)를 적용하면 루프처럼 스택을 더 안 쌓을 수 있다.
  (단, C/C++에서는 컴파일러/옵션/디버그/예외 처리 등에 따라 TCO가 항상 보장되진 않음)

꼬리 재귀 예시:
    return fact_tail(n - 1, n * acc);

틀린(꼬리 재귀가 아닌) 예시:
    return n * fact_normal(n - 1); // 재귀 호출 "뒤"에 곱셈이 남아있음 -> 스택 프레임 유지 필요
*/

#include <iostream>

// ============================= not tail recursion ============================

long long fact_normal(int n)
{
    if (n <= 1)
        return 1;
    // 재귀 호출 이후에 "n * (결과)"를 해야 함 -> 마지막 동작이 아님
    return n * fact_normal(n - 1);
}

// =============================== tail recursion ==============================

long long fact_tail(int n, long long acc)
{
    if (n <= 1)
        return acc;
    // 재귀 호출이 "마지막 동작" -> 꼬리 재귀
    return fact_tail(n - 1, acc * n);
}

// ======================== another tail recursion example =====================
// sum(1..n) with accumulator

long long sum_tail(int n, long long acc)
{
    if (n <= 0)
        return acc;
    return sum_tail(n - 1, acc + n); // 꼬리 재귀
}

// =========================== helper / demonstration ==========================

void demo_factorial(int n)
{
    std::cout << "[factorial]\n";
    std::cout << "normal : " << fact_normal(n) << "\n";
    std::cout << "tail   : " << fact_tail(n, 1) << "\n";
}

void demo_sum(int n)
{
    std::cout << "[sum 1..n]\n";
    std::cout << "tail   : " << sum_tail(n, 0) << "\n";
}

// =================================== main ====================================

int main()
{
    int n = 10;

    demo_factorial(n);
    std::cout << "\n";
    demo_sum(n);

    return 0;
}
