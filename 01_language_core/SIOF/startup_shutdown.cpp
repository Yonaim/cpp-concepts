/*
    - 명시적인 순서로 초기화(startup) 및 정리 작업(shutdown)을 수행하는 클래스를 선언
    - Subsystem들을 감싸는 큰 Engine 클래스

    방법1) Subsystem들을 전역/정적 "포인터"로 선언
    - Engine 클래스는 startup, shutdown 함수에서 초기화/정리 작업 수행
    - SIOF 없음

    방법2) Engine 객체가 Subsystem을 "포인터"로 보유
    - 가장 정석에 해당
    - raw pointer보다는 unique ptr로 선언하는 편

    방법3) Engine 객체가 Subsystem을 "값"으로 보유
    - subsystem 객체의 생성 순서가 멤버 선언 순서로 고정됨
    - 이런 단점 때문에 값 소유는 잘 안 씀
*/

// 아래 예시는 방법1에 해당

// ======================= Subsystems =============================
struct Texture2DManager
{
    Texture2DManager() {}
};

struct TextureManager
{
    TextureManager(Texture2DManager &t2d) {}
};

// 다른 TU에 다음과 같이 선언한다. (Subsystems.cpp)
Texture2DManager *g2dTexture;
TextureManager   *gTexture;

// ========================= Engine ===============================

struct RenderManager
{
    void StartUp()
    {
        g2dTexture = new Texture2DManager();
        gTexture = new TextureManager(*g2dTexture);
    }

    void Shutdown()
    {
        delete gTexture;
        delete g2dTexture;
    }
};
