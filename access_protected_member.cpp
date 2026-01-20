#include <iostream>

/*
    - Base의 protected 멤버:
        Dericed 클래스 내부에서는 접근 가능, 외부에서는 접근 불가
    - using 선언
        - 접근 레벨을 public으로 끌어올림
        - (1) 오버로드 숨김을 풀거나, (2) 
    - 같은 이름의 함수를 Derived에서 선언하면 Base쪽 이름이 가려짐 (name hiding)
*/

class Base
{
  protected:
    int  base_v;
    void base_func() { std::cout << __func__ << std::endl; }
    void base_using_declare_test() { std::cout << __func__ << std::endl; }
    void base_protected() { std::cout << __func__ << std::endl; }
    void func_redeclaration() { std::cout << "Original\n"; }
};

class Derived : public Base
{
  protected:
    int derived_v;

  public:
    // 파생 클래스는 base 클래스의 protected 멤버에 접근 가능
    void d_access_to_member_variable() { std::cout << base_v << std::endl; } // OK
    void d_access_to_member_func() { base_func(); }                           // OK

    // Derived 클래스의 public로 끌어올리기
    using Base::base_using_declare_test;

    // 같은 시그니처의 함수를 재선언할시, name hiding 발생
    void func_redeclaration() { std::cout << "Redeclaration!!!\n"; }
};

int main()
{
    Derived d;

    // Base의 protected 멤버: 외부에서는 접근 불가!!!!
    // std::cout << d.base_v << std::endl;
    // std::cout << d.base_func() << std::endl;

    // 파생 클래스는 base 클래스의 protected 멤버에 접근 가능
    d.d_access_to_member_variable();
    d.d_access_to_member_func();

    // using 키워드로 끌어올리기 가능
    d.base_using_declare_test();

    // 같은 시그니처의 함수를 재선언할시, name hiding 발생
    d.func_redeclaration();
}
