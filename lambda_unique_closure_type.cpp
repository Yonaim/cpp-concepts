#include <iostream>

int main()
{
    {
        auto f1 = []() { std::cout << "Hi" << std::endl; };
        auto f2 = []() { std::cout << "Hello" << std::endl; };

        f1();
        f2();
        std::cout << typeid(42).name() << std::endl;
        std::cout << typeid(f1).name() << std::endl;
        std::cout << typeid(f2).name() << std::endl;

        if (std::is_same_v<decltype(f1), decltype(f2)>)
            std::cout << "same" << std::endl;
        else
            std::cout << "Does not same" << std::endl;
    }

    std::cout << "===========================================\n";

    {
        int  a = 42;
        auto f1 = [&a](int n) // capture
        {
            a += 5;
            std::cout << a + n << std::endl;
        };
        f1(10);
        std::cout << "a: " << a << std::endl;
    }
}
