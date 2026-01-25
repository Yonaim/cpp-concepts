#include <iostream>

/*
    new/delete 정리

    1) ::operator new / ::operator delete (함수)
        = (raw 메모리 할당/해제)

        ex) void* ::operator new(size_t n);
            void  ::operator delete(void* p) noexcept;

    2) new / delete (표현식)
        = (raw 메모리 할당/해제) + (생성자/소멸자 호출)

        ex)
            new T(args...)        : (할당) + (T 생성)
            delete p              : (T 소멸) + (해제)

        사실상 C++스타일의 malloc, free라고 보면 됨

    3) placement new (함수 + 표현식)
        = 지정한 주소에 생성만 수행

        1) placement operator new 함수 호출 (전역일시, 아무것도 안 함)
        2) 그 mem 위치에 생성자 호출

        ex) new (p) T(args...)    : (p에 T 생성)
            p->~T();              : 소멸자 직접 호출
*/

/*
    (대표) operator new/delete 함수 시그니처
    - void* operator new(std::size_t n);
    - void  operator delete(void* p) noexcept;

    (대표) placement operator new 함수 시그니처
    - void* operator new(std::size_t n, void* place) noexcept;
    // 보통 place를 그대로 반환
*/

/*
    placement new (위치 지정 new)
    - 메모리는 sizeof(myType)만큼 미리 확보가 되어있어야 한다.
    - 메모리 관리와 객체 수명 관리를 분리할 수 있음!!!
    - 큰 raw memory 풀을 쪼개 사용하여 효율 향상 가능
*/

using namespace std;

struct myType
{
    int some1;
    int some2;

    myType() { cout << "myType constructor\n"; }
    ~myType() { cout << "myType destructor\n"; }

    void operator delete(void *p)
    {
        cout << "myType::operator delete\n";
        ::operator delete(p);
    }

    // 오직 메모리 할당만 하고 생성자 호출은 하지 않음
    void *operator new(size_t n)
    {
        cout << "myType::operator new " << n << "\n";
        return ::operator new(n);
    }

    // placement new용 ::operator new
    // 실질적으로 아무것도 하지 않고 주소를 그대로 반환
    void *operator new(size_t n, void *place)
    {
        cout << "myType::operator new (placement) " << n << "\n";
        return place;
    }
};

// 다양한 객체, 생성 파괴를 실험
int main()
{
    cout << "\n============== Create t1 ===============\n";
    // 메모리 할당 + 생성자 호출을 묶어서 수행
    // 메모리 할당 함수(::operator new)는 dynamic dispatch로 호출
    myType *t1 = new myType;

    cout << "\n============== Create t2 ===============\n";
    // 메모리 할당 + 생성자 호출을 별도로 수행
    myType *t2 = (myType *)::operator new(sizeof(myType)); // 메모리 할당
    t2 = new (t2) myType;
    // 단순 예제이며, 일반적으로 이렇게 사용하지 않음 (new 표현식에 비해 득이 없음)

    cout << "\n============== Create t3, t4 ===============\n";

    void *mem = myType::operator new(sizeof(myType) * 2); // t3, t4를 위한 메모리 확보
    // 메모리 할당과 생성자 호출을 별도로 수행
    // 연속된 메모리에 placement new를 수행
    myType *first = static_cast<myType *>(mem);
    myType *t3 = new (first) myType;
    myType *t4 = new (first + 1) myType;

    cout << "\n============== Destroy (automatic) ===============\n";
    // 소멸자 호출 + 메모리 해제를 묶어서 수행
    delete t1;

    cout << "\n============== Destroy (manually) ===============\n";
    // 소멸자 호출 + 메모리 해제를 별도로 수행
    t2->~myType();
    ::operator delete(t2); // myType::operator delete가 아닌 전역으로 직접 지정

    return 0;
}
