
/*
    - template 인자에 해당 클래스를 상속 받는 derived 타입을 넣음
        -> base 클래스가 derived 클래스를 알 수 있다
    - Base 클래스에 공통 로직(오버로딩할 함수) 및 캐스팅용 함수 정의
        - 템플릿 인자로 인해 컴파일 타임에 타입을 알 수 있다!!! => static_cast

    => 컴파일타임 연산만으로 virtual (다형성)을 구현할 수 있다.
*/

#include <iostream>

template <class WinType>
class Window
{
  public:
    // public API
    void msgLoop()
    {
        // ...
        winType().onClickImpl();
        // ...
    }

  private:
    // 오버로딩 될 함수 (virtual의 대상)
    void onClickImpl() { std::cout << "Window default click\n"; }

    // 캐스팅용 함수 (static 캐스팅)
    WinType &winType() { return static_cast<WinType &>(*this); }
};

class FrameWindow : public Window<FrameWindow>
{
  public:
    void onClickImpl() { std::cout << "FrameWindow click\n"; }
};

class PopupWindow : public Window<PopupWindow>
{
  public:
    void onClickImpl() { std::cout << "PopupWindow click\n"; }
};

int main()
{
    FrameWindow fw;
    PopupWindow pw;

    fw.msgLoop();
    pw.msgLoop();
}