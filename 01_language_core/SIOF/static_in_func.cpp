
/*
    - SIOF (Static Initialization Order Fiasco)의 해결방법 중 하나
    - 함수 내부에 선언된 static 변수는 '함수가 최초로 호출될 때' 생성된다
    - 즉, static storage duration 변수의 초기화 시점을 내가 정할 수 있다 (원할 때 함수 호출하기)

    문제점: 소멸 순서는 제어할 수 없다!
        정적 객체 A에 의존하고 있는 정적 객체 B가 있을 때,
        A 먼저 소멸되어버리면 B가 곤란해짐
*/

struct PrimitiveManager
{
    PrimitiveManager() {}

    static PrimitiveManager &get()
    {
        static PrimitiveManager singleton;
        return singleton;
    }
};

struct TextureManager
{
    TextureManager() {}

    static TextureManager &get()
    {
        static TextureManager singleton;
        return singleton;
    }
};

// RenderManager는 오브젝트 매니저와 텍스처 메니저에 의존한다.
struct RenderManager
{
    RenderManager()
    {
        // 의존하는 두 객체를 확실하게 먼저 초기화
        PrimitiveManager::get();
        TextureManager::get();

        // 이제 RenderManager 시작!
        // ...
    }

    static RenderManager &get()
    {
        static RenderManager singleton;
        return singleton;
    }

    ~RenderManager()
    {
        // 전부 종료
    }
};
