# C++ Variables: Storage Duration, Scope, Linkage

변수를 나누는 세 가지의 기준이 있다.

1. storage duration (저장 수명)
2. scope
3. linkage

> 중요한 포인트: 스코프는 “보이는 범위”, 저장 수명은 “살아있는 기간”이라 서로 독립
> 예: 함수 내부 `static int x;` 는 block scope + static storage duration.

# Storage duration (저장 수명)

## 1. static storage duration

- '프로그램의 시작~종료'까지 살아있음

- 대상:
    - 전역/네임스페이스 scope 변수
    - static 전역/네임스페이스 scope 변수 
    - 함수 안 static 지역 변수
        - 해당 함수가 처음 호출되는 순간에 생성 (delayed initialization, 지연 초기화)
    - 클래스 내부 static 멤버 변수

- `.data` 혹은 `.bss` 영역에 놓임
- 전역 자체도 namespace::에 해당
- static 전역/네임스페이스 scope 변수는 해당 TU 내부에서만 접근 가능
- SIOF 문제는 서로 다른 TU에 속한 static storage duration 변수 때문에 생김
    - 같은 TU의 static storage duration 변수는 위에서부터 순서대로 초기화
    - 그러나 다른 TU이면 순서가 보장되지않음

**Initialization 관점 — 언제/어떻게 초기화되나**

static storage duration 객체는 초기화 방식이 매우 중요함!!!

- static initialization
    - 0 초기화 + 컴파일타임 상수 초기화 (zero-initialization)
    - 대체로 안전/순서 영향 적음

- dynamic initialization
    - 실행 시 코드가 돌며 초기화
    - 이게 TU 간 순서 미정이라 SIOF의 원인

## 2. automatic storage duration

- 지역변수 (local variable)
- 블록 집입시 생성되고, 종료시 소멸됨
- `stack` 영역에 저장

## 3. dynamic storage duration

-`new/malloc`로 얻는 메모리
- 할당부터 해제까지 수명을 사용자가 결정
---

# Scope

1) block scope: `{}` 안에서만 보임 (대부분의 지역 변수)
2) function parameter scope: 함수 인자
3) namespace scope: 전역/네임스페이스 레벨
4)  class scope: 클래스 멤버 이름

---

# Linkage

다른 번역 단위(TU)에서 같은 이름이 같은 객체로 연결되는가?

1) external linkage: 다른 TU에서도 같은 이름으로 참조 가능
    -  전역 변수 `int g;`

2) internal linkage: 같은 TU 안에서만
    - `static int g;`, 익명 namespace의 변수

3) no linkage
    - 지역 변수 대부분
