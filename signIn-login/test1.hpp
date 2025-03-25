/*
	이런 식으로 헤더파일에 클래스나 메서드만 선언해놓고 main.cpp에서 불러와서 쓰면 됨
*/
#pragma once
#include <iostream>

class Test1 {
public:
	void showTest1() {
		std::cout << "test1.hpp Test1 class showTest1()" << std::endl;
	}
};