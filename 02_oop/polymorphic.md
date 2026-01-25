- 상속이랑 다형성은 별개, 상속했다고 자동으로 다형 클래스가 되는 것이 아님
- 다형 클래스: 런타임 다형성(subtype polymorphism) 만족

- C++에서 런타임 다형성(subtype polymorphism):
    - `Base- / Base&`로 호출했을 때 실제 객체가 Derived면 Derived 함수가 실행되는 것
    - 즉 override가 되어야한다.

- 런타임 다형성을 충족하지 않는 클래스는 사실상 hiding(숨김) 처리만 하는 셈
- `dynamic_cast`, `typeid(*base_ptr)` 같은 RTTI 계열도 vtable 내부/주위의 메타데이터가 필요
