
/*
    Curiously Recurring Template Pattern (CRTP)
    - "묘하게 반복되는 템플릿 패턴"
    - Base 클래스의 템플릿 인자로 Derived 타입이 들어가는 묘한 패턴
        예) Derived : Base<Derived>
    - 컴파일 타임에 Derived 타입을 알고 동작할 수 있음!!!
        - 런타임 다형성(virtual) 없어도 됨!
*/

