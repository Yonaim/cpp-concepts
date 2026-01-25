/*
    - vptr: 각 객체가 들고 있는 vtable 포인터(보통 객체 레이아웃 맨 앞에 위치)
    - vtable: 타입(클래스)당 하나씩 있는 '함수 포인터 테이블' (정적 영역에 존재)
    - type_info: 타입(클래스)당 하나씩 있는 '타입 메타 데이터' (정적 영역에 존재)
        - RTTI용, 일반적으로 vtable 생성시 내부나 근처에 둠
    - 호출 시 vtable 안에서 '탐색'은 안 함
        => 컴파일 타임에 정해진 슬롯 인덱스(=offset)로 바로 접근해서 간접 호출

    - 생성자/소멸자에서 vptr을 알맞게 세팅한다
    - (단일 상속 기준) Base*로 호출하는 virtual 함수는 “Base가 정의한 슬롯 번호”를 사용
        => Derived가 새로 추가한 virtual 함수는 보통 Base 슬롯들 뒤에 추가됨
    - 주의: 다중 상속/가상 상속이면 vptr/vtable이 여러 개일 수 있고 레이아웃도 ABI에 따라 더
   복잡해진다.
*/

/*
    [가상 호출의 비용]

    - 메모리 할당
        - 객체당: vptr 1개
        - 타입당: vtable / RTTI(typeinfo) 같은 정적 데이터

    - 간접 호출(indirect call):
        1) vptr 포인터 값 load
        2) vtable 슬롯 (함수 포인터) 값 laod
        3) load한 함수 포인터 값으로 indirect call

    - 최적화:
        - 컴파일러가 동적 타입을 확정할 수 있으면 devirtualization으로 direct call로 바뀜

    *indirect call이란?
    - 호출할 함수 주소를 메모리에서 읽어온 다음 그 주소로 점프(call)하는 호출
    - 호출 대상이 자주 바뀌면 분기 예측/ICache 측면에서 손해가 커질 수 있음
*/

/*
    가상 호출 제거(=devirtualization) 최적화
    : 가상 함수의 구현(어떤 타입의 override)이 뭔지 컴파일 타임에 확정할 수 있는 경우,
    컴파일러가 가상 호출을 제거하고 정적 호출하도록 최적화할 수 있다.
*/

#include <cstddef> // std::ptrdiff_t
#include <iostream>
#include <typeinfo> // std::type_info, typeid

struct Base;
struct Derived;

// ============================================================================

typedef void (*slot_fn)(Base *);

// Base 슬롯: speak + 일반 함수포인터 2개 + (delete 재현용) deleting dtor
enum BaseSlotIndex
{
    SLOT_SPEAK = 0,
    SLOT_BASE_FN1,
    SLOT_BASE_FN2,
    SLOT_DELETE_DTOR,
    BASE_SLOT_COUNT
};

// Derived 슬롯: Base 슬롯 리스트 + 추가 일반 함수포인터 1개
enum DerivedSlotIndex
{
    // Base 슬롯과 "인덱스 동일" 유지
    SLOT_D_SPEAK = 0,
    SLOT_D_BASE_FN1,
    SLOT_D_BASE_FN2,
    SLOT_D_DELETE_DTOR,
    // Derived 추가 슬롯
    SLOT_DERIVED_EXTRA,
    DERIVED_SLOT_COUNT
};

struct VTableBase
{
    std::ptrdiff_t        offset_to_top; // 여기선 0 고정
    const std::type_info *typeinfo;      // RTTI
    slot_fn               slots[BASE_SLOT_COUNT];
};

struct VTableDerived
{
    std::ptrdiff_t        offset_to_top; // 여기선 0 고정
    const std::type_info *typeinfo;      // RTTI
    slot_fn               slots[DERIVED_SLOT_COUNT];
};

// ============================================================================

struct Base
{
    const VTableBase *vptr;

    Base();
    ~Base();

    void hello() { std::cout << "[manual] Base::hello (non-virtual)\n"; }
};

struct Derived : Base
{
    int extra;

    Derived();
    ~Derived();

    void hello() { std::cout << "[manual] Derived::hello (hides Base::hello)\n"; }
};

// ============================================================================
// 슬롯 구현 함수들 (전방 선언)
// ============================================================================

void base_speak(Base *);
void base_fn1(Base *);
void base_fn2(Base *);
void base_delete_dtor(Base *);

void derived_speak(Base *);
void derived_fn1(Base *);
void derived_fn2(Base *);
void derived_delete_dtor(Base *);
void derived_extra(Base *);

// vtable 정적 객체 (static storage duration)
const VTableBase BASE_VT = {
    0, &typeid(Base), {&base_speak, &base_fn1, &base_fn2, &base_delete_dtor}};

const VTableDerived DERIVED_VT = {
    0,
    &typeid(Derived),
    {&derived_speak, &derived_fn1, &derived_fn2, &derived_delete_dtor, &derived_extra}};

// ============================================================================

void base_speak(Base *) { std::cout << "[manual] Base::speak\n"; }
void base_fn1(Base *) { std::cout << "[manual] Base::fn1\n"; }
void base_fn2(Base *) { std::cout << "[manual] Base::fn2\n"; }

void derived_speak(Base *self)
{
    Derived *d = static_cast<Derived *>(self);
    (void)d;
    std::cout << "[manual] Derived::speak\n";
}

void derived_fn1(Base *self)
{
    Derived *d = static_cast<Derived *>(self);
    (void)d;
    std::cout << "[manual] Derived::fn1 (override)\n";
}

void derived_fn2(Base *self)
{
    Derived *d = static_cast<Derived *>(self);
    (void)d;
    std::cout << "[manual] Derived::fn2\n";
}

void derived_extra(Base *self)
{
    Derived *d = static_cast<Derived *>(self);
    (void)d;
    std::cout << "[manual] Derived::extra_fn\n";
}

void base_delete_dtor(Base *self)
{
    self->~Base();
    ::operator delete(self);
}

void derived_delete_dtor(Base *self)
{
    Derived *d = static_cast<Derived *>(self);
    d->~Derived();
    ::operator delete(d);
}

/*
    생성자/소멸자에서 vptr 세팅
    : 현재 실행 중인 클래스 버전으로 dynamic dispatch를 수행하기 위함
        (특히 base 타입인 상태에서 derived 함수를 호출할 경우 문제가 생길 수 있음)
*/

Base::Base() : vptr(&BASE_VT) { std::cout << "[manual] Base::Base (vptr=Base)\n"; }

Base::~Base()
{
    vptr = &BASE_VT;

    // doing some cleanup...

    std::cout << "[manual] Base::~Base (vptr<-Base)\n";
}

Derived::Derived() : Base(), extra(42)
{
    // Base* 관점(vptr 타입)은 VTableBase* 로 본다(파생 vtable의 prefix를 가리키게)
    vptr = reinterpret_cast<const VTableBase *>(&DERIVED_VT);
    std::cout << "[manual] Derived::Derived (vptr=Derived)\n";
}

Derived::~Derived()
{
    vptr = reinterpret_cast<const VTableBase *>(&DERIVED_VT);

    // doing some cleanup...

    std::cout << "[manual] Derived::~Derived (vptr=Derived)\n";
}

// ============================================================================
// main
// ============================================================================

int main()
{
    std::cout << "\n--- new Base; as Base* ---\n";
    Base *b = new Base();
    std::cout << "[manual] dynamic type = " << b->vptr->typeinfo->name() << "\n";
    b->vptr->slots[SLOT_SPEAK](b);
    b->vptr->slots[SLOT_BASE_FN1](b);
    b->vptr->slots[SLOT_BASE_FN2](b);
    b->hello();

    std::cout << "\n--- new Derived; hold as Base* ---\n";
    Base *p = new Derived();
    std::cout << "[manual] dynamic type = " << p->vptr->typeinfo->name() << "\n";
    p->vptr->slots[SLOT_SPEAK](p);
    p->vptr->slots[SLOT_BASE_FN1](p);
    p->vptr->slots[SLOT_BASE_FN2](p);
    p->hello();

    std::cout << "\n--- Derived extra slot call (Derived vtable view) ---\n";
    {
        const VTableDerived *vt = reinterpret_cast<const VTableDerived *>(p->vptr);
        vt->slots[SLOT_DERIVED_EXTRA](p);
    }

    std::cout << "\n--- delete through Base* (virtual delete 재현) ---\n";
    b->vptr->slots[SLOT_DELETE_DTOR](b);
    p->vptr->slots[SLOT_DELETE_DTOR](p);

    std::cout << "\n--- stack object (주의: delete 하면 안 됨) ---\n";
    Derived stack_d;
    Base   *as_base = &stack_d;
    std::cout << "[manual] dynamic type = " << as_base->vptr->typeinfo->name() << "\n";
    as_base->vptr->slots[SLOT_SPEAK](as_base);
    as_base->vptr->slots[SLOT_BASE_FN1](as_base);
    as_base->vptr->slots[SLOT_BASE_FN2](as_base);

    {
        const VTableDerived *vt = reinterpret_cast<const VTableDerived *>(as_base->vptr);
        vt->slots[SLOT_DERIVED_EXTRA](as_base);
    }

    as_base->hello();
}
