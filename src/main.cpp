#include <iostream>
#include <thread>

void functio_1(std::string value)
{
	std::cout << "Multithreading " << value << std::endl;
}

class Fctor
{
public:
	void operator()()
	{
		for(int i = 0; i>-100; i--)
			std::cout << "From Fctor: " << i << std::endl;
	}
};

int main()
{
	Fctor fctor;
	std::thread t1 (fctor); // starts running

	for(int i = 0; i<100; i++)
		std::cout << "From main: " << i << std::endl;

	std::thread t2 (functio_1, "2");
	
	t1.detach(); // --daemon process

	if(t2.joinable())
		t2.join();

	return 0;
}