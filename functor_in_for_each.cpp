#include <functional>
#include <iostream>
#include <algorithm>

// callable : functor, function pointer, lambda

// functor1
struct Ssimple
{
	void operator()(int num)
	{
		std::cout << "number : " << num << std::endl;
	}
};

// functor2
struct Scomplex
{
	int sum;
	Scomplex()
	{
		sum = 0;
	}
	void operator()(int num)
	{
		sum += num;
		std::cout << "sum : " << sum << std::endl;
	}
};

int main()
{
	Ssimple simple;
	simple(42);

	Scomplex complex;
	complex(42);
	complex(7);

	std::vector<int> vec{1, 2, 3, 4};

	std::for_each(vec.begin(), vec.end(), Ssimple());  // functor1
	std::for_each(vec.begin(), vec.end(), Scomplex()); // functor2

	Scomplex complex2;

	// complex2를 내부에서 복사하기 때문에 sum의 값은 그대로임
	std::for_each(vec.begin(), vec.end(), complex2); 
    std::cout << "Now sum is " << complex2.sum << "\n\n";

    // std::ref()를 사용하면 레퍼런스로 넘길 수 있다. (원래의 sum 값이 바뀜)
	std::for_each(vec.begin(), vec.end(), std::ref(complex2));
	std::cout << "Now sum is " << complex2.sum << "\n\n";

    // 혹은 for_each가 계산 결과의 복사본을 반환한다는 점을 이용한다
	Scomplex complex3 = std::for_each(vec.begin(), vec.end(), complex2);
	std::cout << "Complex 3's sum is " << complex3.sum << "\n\n";
}
