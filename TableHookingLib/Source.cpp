#include <iostream>
#include "Library.hpp"

__declspec(noinline) int add(int a1, int a2)
{
	return a1 + a2;
}

__declspec(noinline) int sub(int a1, int a2)
{
	return a1 - a2;
}

int main()
{
	auto hook = t_hook(&add, &sub);

	std::cout << add(5, 5) << '\n';
	std::cout << sub(5, 5) << '\n';
	hook.enable();
	std::cout << add(5, 5) << '\n';
	hook.disable();
	std::cout << add(5, 5) << '\n';
}