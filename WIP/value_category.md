* glvalue = generalized lvalue (일반화된 lvalue)
  → “객체를 지칭하는 표현식”을 lvalue로만 부르기엔 범위가 넓어서( xvalue도 포함 ) 만든 이름.

* prvalue = pure rvalue (순수 rvalue)
  → “값 그 자체(임시 값)” 쪽 rvalue.

* xvalue = expiring value (만료(expiring)하는 값)
  → “곧 소멸/이동 대상으로 취급되는 객체”를 지칭하는 값.

---

# C++ Value Category 학습노트 (glvalue / prvalue / xvalue)

`value category`는 C++에서 **표현식(expression)**을 “값의 성격”으로 분류하는 규칙이다.
핵심은 “이 표현식이 **객체를 지칭하냐(정체성/identity)**, 아니면 **값만 만들고 끝나냐**”다.

---

## 1) 큰 그림 (분류 & 관계)

C++ 표현식은 아래 3개가 기본 축이다:

* **glvalue**: 어떤 **객체를 지칭**한다 (identity 있음, “그 객체”라고 가리킬 수 있음)
* **prvalue**: **순수 값**(계산 결과/임시 값). 객체를 지칭한다기보다 “값” 자체
* **xvalue**: **곧 소멸/이동 대상으로 취급되는 객체**를 지칭하는 glvalue (eXpiring)

관계는 이렇게 외우면 됨:

* **glvalue = lvalue + xvalue**
* **rvalue = prvalue + xvalue**

즉,

* lvalue는 glvalue의 한 종류
* xvalue는 glvalue이면서 동시에 rvalue 쪽 성격도 가진다

---

## 2) 왜 중요한가? (실전에서 터지는 포인트만)

value category는 아래를 결정/유도한다:

### (1) 오버로드 선택 (특히 `T&` vs `T&&`)

* `f(T&)`는 보통 **lvalue**만 받는다
* `f(T&&)`는 보통 **rvalue(prvalue/xvalue)**를 받는다
  → “복사 vs 이동” 선택이 여기서 갈림

### (2) move semantics가 “언제” 발동하나

* `std::move(x)`는 **x를 xvalue로 만들어서** 이동 생성자/이동 대입이 후보가 되게 한다
* 즉 “이동이 가능한 상황으로 표현식을 바꿔주는 장치”가 value category다

### (3) `decltype`, `auto&&` 같은 타입 추론에서 결과가 달라짐

* `decltype(expr)`는 expr이 lvalue냐 rvalue냐에 따라 `T` vs `T&` 같은 결과가 갈린다
* `auto&&`는 forwarding reference로서 value category를 “전달”한다

### (4) `typeid`의 “동적 타입” 조건

* `typeid(expr)`에서 **동적 타입(runtime type)**을 보려면
  `expr`이 **polymorphic class type의 glvalue**여야 한다
  (virtual 있는 기반 클래스 참조/역참조 같은 케이스)

---

## 3) 예시로 감 잡기 (대표 케이스만)

```cpp
int x = 10;
int* p = &x;

x;        // glvalue (lvalue) : x라는 객체를 지칭
*p;       // glvalue (lvalue) : 포인터가 가리키는 객체를 지칭
++x;      // glvalue (lvalue) : 증가 후 '그 객체'를 다시 지칭

x + 1;    // prvalue          : 계산 결과 값(임시)
42;       // prvalue          : 리터럴 값(임시 값)

std::move(x); // xvalue (glvalue) : x를 "이동 대상으로 취급"하는 객체 지칭
```

자주 나오는 추가 예:

```cpp
struct S { int m; };

S s{};
s.m;        // glvalue(lvalue) : 멤버 객체 지칭
S{}.m;      // xvalue(glvalue) 또는 상황에 따라(표준 규칙) rvalue쪽 성격 : 임시 객체의 멤버
```

---

## 4) glvalue / prvalue / xvalue 빠른 판별 감각

### glvalue(대부분 lvalue)

* “이 표현식이 **어떤 객체를 가리키는가?**”
* 예: 변수명, `*p`, `arr[i]`, `obj.member`, `++x`

### prvalue

* “그냥 **계산 결과 값**이냐?”
* 예: `a+b`, `makeT()`, 리터럴 `42`

### xvalue

* “객체를 가리키긴 하는데, **이제 이동해도 되는 대상**으로 취급하나?”
* 예: `std::move(x)`, `static_cast<T&&>(x)`
* 느낌: “정체성은 있는데(explicit히 지칭), rvalue처럼 다룬다”

---

## 5) 헷갈림 방지: `std::move`는 “이동 실행”이 아니다

```cpp
std::string s = "hi";
auto t = std::move(s);
```

* `std::move(s)`는 **s를 xvalue로 캐스팅**했을 뿐
* 실제 이동은 `t`를 만들 때(이동 생성자/대입이 선택될 때) 발생
* 이동 후 `s`는 “유효하지만 내용은 보장 X(보통 비었을 수 있음)” 상태가 될 수 있음

---

## 6) 한 줄 요약(면접용)

* **glvalue**: “객체를 지칭한다(정체성 있음)”
* **prvalue**: “값만 만든다(임시 결과)”
* **xvalue**: “객체를 지칭하지만 이동 대상으로 취급되는 glvalue”

---

원하면 이 스타일 그대로 이어서:

* `T& / const T& / T&&` 바인딩 규칙을 **예시 6개로** 정리해줄까, 아니면
* `decltype((expr))`에서 왜 괄호 하나로 `T` vs `T&` 갈리는지 **한 방에** 정리해줄까?
