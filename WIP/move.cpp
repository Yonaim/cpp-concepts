/*
    [C++ move / rvalue / std::move 한 파일 예제 (실무 감각 + 내부 원리까지)]

    0) move가 "뭔데"?
       - move 자체가 어떤 데이터를 옮기는 마법이 아님.
       - std::move(x)는 x를 "rvalue로 취급하겠다"는 캐스트(정확히는 T&&로의 cast)일 뿐.
       - 실제로 자원을 옮기는 건 "이동 생성자/이동 대입"이 한다.

    1) lvalue / rvalue 감각
       - 이름이 있는 변수는 보통 lvalue.
       - 임시 객체/함수 반환값 같은 건 rvalue인 경우가 많음.
       - 오버로드로 확인 가능: f(T&) vs f(T&&)

    2) Rule of 5 (자원 소유 타입이면 필수급)
       - 소멸자 / 복사 생성 / 복사 대입 / 이동 생성 / 이동 대입
       - move 구현의 핵심: "포인터 훔치고, 원본을 안전한 빈 상태로 만들기"

    3) std::vector가 빠른 이유(중요)
       - reallocation 때 요소를 복사하면 O(N) + 각 복사가 비쌈.
       - 요소가 move 가능하면 "포인터만 훔치는" 이동으로 훨씬 싸짐.
       - 단, move가 예외를 던지면(vector는 강한 예외 보장 때문에) 복사를 택할 수도 있음.
         => 가능하면 move를 noexcept로 만들면 최적화에 유리.

    이 파일은 아래 데모를 순서대로 실행:
      demo_overload_lvalue_rvalue()
      demo_move_is_cast()
      demo_move_ctor_and_assign()
      demo_vector_move_benefit()
*/

#include <cassert>
#include <cstring> // std::memcpy
#include <iostream>
#include <utility> // std::move
#include <vector>

// ------------------------------------------------------------
// 1) lvalue/rvalue 오버로드로 감각 잡기
// ------------------------------------------------------------
static void sink(int &) { std::cout << "sink(int&)  : lvalue\n"; }
static void sink(int &&) { std::cout << "sink(int&&) : rvalue\n"; }

static void demo_overload_lvalue_rvalue()
{
    std::cout << "\n==== demo_overload_lvalue_rvalue ====\n";

    int x = 10;         // x는 이름이 있으므로 lvalue
    sink(x);            // int& 버전
    sink(20);           // 임시값 => rvalue => int&& 버전
    sink(std::move(x)); // x를 rvalue로 취급 => int&& 버전
}

// ------------------------------------------------------------
// 2) std::move는 "이동"이 아니라 "캐스트"다
//    - 이동은 move ctor/assign에서 발생
// ------------------------------------------------------------
struct Tracer
{
    Tracer() { std::cout << "Tracer()\n"; }
    Tracer(const Tracer &) { std::cout << "Tracer(copy)\n"; }
    Tracer(Tracer &&) noexcept { std::cout << "Tracer(move)\n"; }
};

static void take_by_value(Tracer t)
{
    (void)t;
    std::cout << "take_by_value done\n";
}

static void demo_move_is_cast()
{
    std::cout << "\n==== demo_move_is_cast ====\n";

    Tracer a;
    std::cout << "-- pass lvalue (copy expected) --\n";
    take_by_value(a); // copy 가능성이 큼

    std::cout << "-- pass std::move(lvalue) (move expected) --\n";
    take_by_value(std::move(a)); // move 가능성이 큼
}

// ------------------------------------------------------------
// 3) 자원 소유 타입 예제: 이동 생성/대입이 "포인터 훔치기"라는 걸 보여줌
// ------------------------------------------------------------
class Buffer
{
  public:
    Buffer() : data_(nullptr), size_(0) { std::cout << "Buffer()\n"; }

    explicit Buffer(size_t n) : data_(new int[n]), size_(n)
    {
        std::cout << "Buffer(size=" << size_ << ")\n";
        for (size_t i = 0; i < size_; ++i)
            data_[i] = int(i);
    }

    ~Buffer()
    {
        std::cout << "~Buffer(size=" << size_ << ")\n";
        delete[] data_;
    }

    // copy ctor (deep copy)
    Buffer(const Buffer &other) : data_(nullptr), size_(other.size_)
    {
        std::cout << "Buffer(copy)\n";
        if (size_)
        {
            data_ = new int[size_];
            std::memcpy(data_, other.data_, sizeof(int) * size_);
        }
    }

    // copy assign (deep copy)
    Buffer &operator=(const Buffer &other)
    {
        std::cout << "Buffer(copy assign)\n";
        if (this == &other)
            return *this;

        // 새로 복사할 공간 마련 후 교체(강한 예외 보장 느낌)
        int *new_data = nullptr;
        if (other.size_)
        {
            new_data = new int[other.size_];
            std::memcpy(new_data, other.data_, sizeof(int) * other.size_);
        }

        delete[] data_;
        data_ = new_data;
        size_ = other.size_;
        return *this;
    }

    // move ctor (steal)
    Buffer(Buffer &&other) noexcept : data_(other.data_), size_(other.size_)
    {
        std::cout << "Buffer(move)\n";
        other.data_ = nullptr;
        other.size_ = 0;
    }

    // move assign (steal)
    Buffer &operator=(Buffer &&other) noexcept
    {
        std::cout << "Buffer(move assign)\n";
        if (this == &other)
            return *this;

        delete[] data_; // 내 자원 먼저 정리
        data_ = other.data_;
        size_ = other.size_;
        other.data_ = nullptr;
        other.size_ = 0;
        return *this;
    }

    size_t     size() const { return size_; }
    bool       empty() const { return size_ == 0; }
    const int *data() const { return data_; }

  private:
    int   *data_;
    size_t size_;
};

static Buffer make_buffer(size_t n)
{
    Buffer b(n);
    return b; // RVO/NRVO 또는 move
}

static void demo_move_ctor_and_assign()
{
    std::cout << "\n==== demo_move_ctor_and_assign ====\n";

    std::cout << "-- move ctor from temporary(return) --\n";
    Buffer a = make_buffer(5); // 보통 RVO, 아니면 move
    std::cout << "a.size = " << a.size() << "\n";

    std::cout << "-- move ctor from lvalue via std::move --\n";
    Buffer b(3);
    Buffer c = std::move(b); // b 자원을 c가 훔침
    std::cout << "c.size = " << c.size() << "\n";
    std::cout << "b.size = " << b.size() << " (moved-from)\n";
    assert(b.empty()); // moved-from는 "유효하지만 값은 미정"이지만 보통 비워둠

    std::cout << "-- move assign --\n";
    Buffer d(10);
    d = std::move(c);
    std::cout << "d.size = " << d.size() << "\n";
    std::cout << "c.size = " << c.size() << " (moved-from)\n";
}

// ------------------------------------------------------------
// 4) vector가 move(noexcept)를 좋아하는 이유를 로그로 보기
//    - 요소 타입이 move noexcept면 reallocation에서 move를 더 잘 선택함.
// ------------------------------------------------------------
struct Heavy
{
    Heavy() : p(new int[1000]) { std::cout << "Heavy()\n"; }
    ~Heavy()
    {
        delete[] p;
        std::cout << "~Heavy()\n";
    }

    Heavy(const Heavy &) : p(new int[1000]) { std::cout << "Heavy(copy)\n"; }

    Heavy(Heavy &&other) noexcept : p(other.p)
    {
        std::cout << "Heavy(move)\n";
        other.p = nullptr;
    }

    Heavy &operator=(const Heavy &) = delete;
    Heavy &operator=(Heavy &&) = delete;

    int *p;
};

static void demo_vector_move_benefit()
{
    std::cout << "\n==== demo_vector_move_benefit ====\n";

    std::vector<Heavy> v;
    v.reserve(1); // 일부러 작게 잡고 늘리면서 reallocation 유도

    std::cout << "-- push 1 --\n";
    v.emplace_back();

    std::cout << "-- push 2 (likely triggers reallocation, watch move/copy logs) --\n";
    v.emplace_back();

    std::cout << "-- push 3 (more reallocations possible) --\n";
    v.emplace_back();
}

// ------------------------------------------------------------
// main
// ------------------------------------------------------------
int main()
{
    demo_overload_lvalue_rvalue();
    demo_move_is_cast();
    demo_move_ctor_and_assign();
    demo_vector_move_benefit();
    return 0;
}
