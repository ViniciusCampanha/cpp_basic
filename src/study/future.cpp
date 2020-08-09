#include <iostream>
#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>

int factorial(std::future<int>& f)
{
	int res = 1;

	std::cout << "Start factorial."  << std::endl;

	int n = f.get();

	for(int i = n; i > 1; i--)
		res *= i;

	std::cout << "Result is: " << res << std::endl;

	return res;
}

int main()
{
	int x;

	//std::thread t1(factorial, 4);
	//t1.join();

	//std::future<int> future = std::async(factorial, 4); // depends on implementation, same as bellow
	//std::future<int> future = std::async(std::launch::async | std::launch::deferred, factorial, 4); // same as above, depends on implementation

	//std::future<int> future = std::async(std::launch::deferred, factorial, 4); // same thread
	
	std::promise<int> p;
	std::future<int> f = p.get_future();

	std::future<int> future = std::async(std::launch::async, factorial, std::ref(f)); // creates another thread

	// do something else
	std::this_thread::sleep_for(std::chrono::seconds(2));

	p.set_value(10);
	x = future.get();

	// fu.get() can only be called once

	std::cout << "Get from child: " << x << std::endl;

	return 0;
}