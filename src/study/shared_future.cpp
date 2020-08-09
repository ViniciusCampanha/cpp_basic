#include <iostream>
#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <tuple>
#include <chrono>


std::tuple<int, int> factorial(std::shared_future<int> f, int id)
{
	int res = 1;

	std::cout << "Start factorial " << id  << std::endl;

	int n = f.get();

	for(int i = n; i > 1; i--)
		res *= i;

	std::cout << "Process " << id << " finished!" << std::endl;

	if(id == 7)
		std::this_thread::sleep_for(std::chrono::seconds(10));

	return std::make_tuple(id, res);
}

int main()
{
	std::promise<int> p;
	std::future<int> f = p.get_future();
	std::shared_future<int> sf = f.share();

	std::vector<std::future<std::tuple<int, int>>> vFuture;

	for(int i = 0; i < 10; i++)
	{
		vFuture.push_back(std::async(std::launch::async, factorial, sf, i));
	}

	// do something else
	std::this_thread::sleep_for(std::chrono::seconds(2));

	p.set_value(4);

	for(auto& future : vFuture)
	{	
		std::future_status status = future.wait_for(std::chrono::microseconds(1));

		if(status != std::future_status::ready)
			continue;

		std::tuple<int, int> res = future.get();
		std::cout << "Get from child " << std::get<0>(res) << ": " << std::get<1>(res) << std::endl;
	}

	return 0;
}