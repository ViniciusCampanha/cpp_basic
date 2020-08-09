#include <iostream>
#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <tuple>
#include <chrono>
#include <deque>

std::deque<std::tuple<int, int>> _results;

std::condition_variable resultsCondition;

std::mutex _lkResults;
std::mutex _lkCout;

void factorial(std::shared_future<int> f, int id)
{
	int res = 1;

	std::unique_lock<std::mutex> lockerCout(_lkCout);
	std::cout << "Start factorial " << id  << std::endl;
	lockerCout.unlock();

	int n = f.get();

	for(int i = n; i > 1; i--)
		res *= i;	

	if(id == 7)
		std::this_thread::sleep_for(std::chrono::seconds(10));

	lockerCout.lock();
	std::cout << "Process " << id << " aguardando loker do results!" << std::endl;
	lockerCout.unlock();

	std::unique_lock<std::mutex> lockerResults(_lkResults);
	_results.push_back(std::make_tuple(id, res));
	lockerResults.unlock();

	resultsCondition.notify_all();

	lockerCout.lock();
	std::cout << "Process " << id << " finished!" << std::endl;
	lockerCout.unlock();
}

void print_results()
{
	while(1)
	{
		std::unique_lock<std::mutex> lockerResults(_lkResults);

		resultsCondition.wait(lockerResults, [](){ return !_results.empty(); });
		
		std::tuple<int,int> res = _results.front();
		_results.pop_front();

		lockerResults.unlock();

		std::unique_lock<std::mutex> lockerCout(_lkCout);
		std::cout << "Get from thread " << std::get<0>(res) << ": " << std::get<1>(res) << std::endl;					
		lockerCout.unlock();
	}
}



int main()
{
	std::future<void> futurePrint = std::async(std::launch::async, print_results);	

	std::promise<int> p;
	std::future<int> f = p.get_future();
	std::shared_future<int> sf = f.share();

	std::vector<std::future<void>> _vecFuture;

	for(int i = 0; i < 10; i++)
	{
		_vecFuture.push_back(std::async(std::launch::async, factorial, sf, i));
	}

	p.set_value(4);

	std::this_thread::sleep_for(std::chrono::seconds(2));

	return 0;
}