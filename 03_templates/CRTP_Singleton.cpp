/*
    - 싱글톤 타입을 위한 base 클래스를 작성할 때 CRTP을 쓸 수 있다!!
    - Derived 클래스는 이 base 클래스를 상속한다
*/

/*
    Instance가 static 타입 "객체"가 아니라 "포인터"인 이유?
    -> 만약 static 객체로 선언이 된다면 그건 "프로그램 시작 시점"에 생성되는 객체라서,
        SIOF 문제가 발생하기 때문이다 (static이면 클래스는 사실상 namespace 스코프와 같은 역할)
*/
template <class Derived>
class Singleton
{
  private:
    // static 변수이므로 자동으로 0(=NULL) 초기화
    static Derived *Instance;

  protected:
    // Derived 클래스의 생성자에서 호출할 수 있으려면 생성자는 private면 안됨
    Singleton() = default;
    ~Singleton() = default;

  public:
    Singleton(const Singleton &) = delete;
    Singleton &operator=(const Singleton &) = delete;

    static Derived &Get() { return *Instance; }

    static void Create()
    {
        // ...
    }

    static void Destroy()
    {
        // ....
    }
};

// =============================================================================
// 사용 예시
class Engine : public Singleton<Engine>
{
  private:
    Engine()
    {
        // 세팅 작업
    }

    // Singleton 클래스가 Engine 생성자를 호출할 수 있도록 friend로 선언
    // 만약 Engine의 생성자가 public이라면 굳이 friend일 필요는 없음
    friend class Singleton<Engine>;

  public:
    void Run()
    {
        // ...
    }

    void DoSomething()
    {
        // ...
    }
};

int main()
{
    Engine::Get().Create();

    // ...

    Engine::Get().Run();
    Engine::Get().DoSomething();

    // ...

    Engine::Get().Destroy();
};
