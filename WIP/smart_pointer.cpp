/*
    [C++ 스마트 포인터 한 파일 예제 (실무 패턴 중심)]

    0) 왜 쓰나?
       - new/delete 직접 관리 대신 RAII로 "예외/조기 return" 상황에서도 자동 해제.
       - 소유권(누가 책임지고 해제하나)을 타입으로 명확히 표현.

    1) unique_ptr<T> : 단독 소유(기본/권장)
       - 복사 불가, 이동(move)만 가능 -> 소유권 이전이 코드에 드러남.
       - make_unique로 생성 권장.
       - .get()은 "잠깐 접근"일 뿐, 소유권 이전이 아님.

    2) unique_ptr<T, Deleter> : C API 핸들/리소스 RAII화(실무 핵심)
       - FILE*, 소켓, OS 핸들, Vulkan/DirectX 리소스 등 "해제 함수가 따로 있는" 자원에 자주 씀.

    3) shared_ptr<T> : 공유 소유(참조 카운트)
       - 여러 주체가 동일 객체 수명을 공유해야 할 때만 사용.
       - refcount 증감 비용(원자 연산)이 있고, "객체 내부 동기화"는 별개.

    4) weak_ptr<T> : 순환 참조(cycle) 끊기
       - shared_ptr끼리 서로 물면 refcount가 0이 안 돼서 해제가 안 됨.
       - parent<->child, observer(구독) 구조에서 한쪽을 weak_ptr로 둠.

    5) enable_shared_from_this : this를 안전하게 shared_ptr로 얻기
       - 이미 shared_ptr로 관리되는 객체가 비동기 작업 등에 "자기 수명 연장"이 필요할 때 사용.
       - 주의: shared_ptr로 소유되기 전에 shared_from_this() 호출하면 예외(std::bad_weak_ptr).

    이 파일은 아래 데모를 순서대로 실행:
      demo_unique_move()
      demo_unique_custom_deleter_file()
      demo_shared_counts()
      demo_weak_break_cycle()
      demo_enable_shared_from_this()
*/

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

// ---------------------------
// 1) unique_ptr 단독 소유 + move
// ---------------------------
struct Texture
{
    explicit Texture(const char *path) : path(path)
    {
        std::cout << "[Texture] Load: " << path << "\n";
    }
    ~Texture() { std::cout << "[Texture] Free: " << path << "\n"; }
    const char *path;
};

static void demo_unique_move()
{
    std::cout << "\n==== demo_unique_move ====\n";

    auto tex = std::make_unique<Texture>("albedo.png");

    // 소유권 이전(이동)
    std::unique_ptr<Texture> owner = std::move(tex);
    if (!tex)
        std::cout << "tex is null after move\n";

    // 컨테이너에 보관(복사 X, move O)
    std::vector<std::unique_ptr<Texture>> textures;
    textures.push_back(std::make_unique<Texture>("normal.png"));
    textures.push_back(std::make_unique<Texture>("roughness.png"));

    // raw pointer "잠깐 접근"
    Texture *raw = owner.get();
    std::cout << "Using raw (non-owning): " << raw->path << "\n";
}

// ---------------------------
// 2) unique_ptr + 커스텀 deleter (C 핸들 RAII)
// ---------------------------
struct FileCloser
{
    void operator()(std::FILE *f) const noexcept
    {
        if (f)
            std::fclose(f);
    }
};

static void demo_unique_custom_deleter_file()
{
    std::cout << "\n==== demo_unique_custom_deleter_file ====\n";

    std::unique_ptr<std::FILE, FileCloser> fp(std::fopen("smartptr_demo.txt", "wb"));
    if (!fp)
        throw std::runtime_error("fopen failed");

    std::fputs("hello from smart pointers\n", fp.get());
    std::cout << "Wrote to smartptr_demo.txt (FILE* will be closed automatically)\n";
}

// ---------------------------
// 3) shared_ptr 참조 카운트 예시
// ---------------------------
struct Node
{
    explicit Node(int v) : v(v) { std::cout << "[Node] ctor(" << v << ")\n"; }
    ~Node() { std::cout << "[Node] dtor(" << v << ")\n"; }
    int v;
};

static void use_shared(std::shared_ptr<Node> p)
{ // 복사 -> refcount +1
    std::cout << "use_shared(): use_count = " << p.use_count() << "\n";
}

static void demo_shared_counts()
{
    std::cout << "\n==== demo_shared_counts ====\n";

    auto p = std::make_shared<Node>(42);
    std::cout << "after make_shared: use_count = " << p.use_count() << "\n"; // 1

    use_shared(p);
    std::cout << "after call: use_count = " << p.use_count() << "\n"; // 1

    auto q = p;
    std::cout << "after copy: use_count = " << p.use_count() << "\n"; // 2

    q.reset();
    std::cout << "after q.reset: use_count = " << p.use_count() << "\n"; // 1
}

// ---------------------------
// 4) weak_ptr로 cycle 끊기 (A<->B)
// ---------------------------
struct B; // forward

struct A
{
    std::shared_ptr<B> b; // strong
    ~A() { std::cout << "[A] dtor\n"; }
};

struct B
{
    std::weak_ptr<A> a; // weak to break cycle
    ~B() { std::cout << "[B] dtor\n"; }
};

static void demo_weak_break_cycle()
{
    std::cout << "\n==== demo_weak_break_cycle ====\n";

    auto a = std::make_shared<A>();
    auto b = std::make_shared<B>();

    a->b = b;
    b->a = a; // weak

    std::cout << "A use_count = " << a.use_count() << "\n";
    std::cout << "B use_count = " << b.use_count() << "\n";

    // weak_ptr 접근: lock()으로 shared_ptr 얻기(성공 시)
    if (auto locked = b->a.lock())
    {
        std::cout << "weak.lock() success (A is alive)\n";
    }
}

// ---------------------------
// 5) enable_shared_from_this : 자기 자신을 shared_ptr로 얻기
// ---------------------------
struct Session : std::enable_shared_from_this<Session>
{
    void start_async_like()
    {
        // 비동기 작업에서 this 수명 보장하려고 self를 잡는 패턴
        auto self = shared_from_this();
        std::cout << "[Session] start_async_like, use_count = " << self.use_count() << "\n";
    }
    ~Session() { std::cout << "[Session] dtor\n"; }
};

static void demo_enable_shared_from_this()
{
    std::cout << "\n==== demo_enable_shared_from_this ====\n";

    auto s = std::make_shared<Session>();
    s->start_async_like();
}

// ---------------------------
// main
// ---------------------------
int main()
{
    try
    {
        demo_unique_move();
        demo_unique_custom_deleter_file();
        demo_shared_counts();
        demo_weak_break_cycle();
        demo_enable_shared_from_this();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
