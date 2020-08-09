#include <iostream>
#include <string>

template <typename T>
class Foo
{
	T tVar;
public:
	Foo(T t) : tVar(t){}
	Foo(){}
};

class FooDerived: public Foo<std::string>{};

class A
{
public:
	int data;

	virtual void a(){}
	virtual void b(){}
	virtual void c(){}

};
int main()
{
	//FooDerived d;

	std::cout << sizeof(A) << std::endl;
	return 0;
}