# Static Initialization Order Fiasco (SIOF)

- 서로 다른 번역 단위(TU)에 있는 static storage duration 객체(전역/정적 객체)들의 동적 초기화(dynamic initialization) 순서가 미정이라서 생기는 버그
- 전역/정적 객체는 `main()` 전에 초기화될 수 있음
- 그런데 A.cpp의 전역 객체 생성자에서 B.cpp의 전역 객체를 사용하면 B가 아직 초기화되지 않았을 수도 있음
- 결과: 미초기화 상태 접근, 크래시, 랜덤 동작(플랫폼/옵션/링커에 따라 달라짐)

https://en.cppreference.com/w/cpp/language/siof.html
https://tithingbygame.tistory.com/253
