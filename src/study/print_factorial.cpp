#include <iostream>
#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <tuple>
#include <chrono>
#include <deque>

std::vector<std::thread> _threads;
std::deque<std::tuple<int, int>> _results;

std::condition_variable resultsCondition;

std::condition_variable condFuture;

std::mutex _lkResults;
std::mutex _lkCout;
std::mutex _lkFuture;

void factorial(std::shared_future<int> f, int id)
{
	int res = 1;

	std::unique_lock<std::mutex> lockerCout(_lkCout);
	std::cout << "Start factorial " << id  << std::endl;
	lockerCout.unlock();

	std::unique_lock<std::mutex> lockerFuture(_lkFuture);
	condFuture.wait(lockerFuture, [&](){ return f.wait_for(std::chrono::microseconds(1)) == std::future_status::ready; });

	int n = f.get();

	lockerFuture.unlock();

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
	std::thread t(print_results);
	_threads.push_back(std::move(t));

	std::promise<int> p;
	std::future<int> f = p.get_future();
	std::shared_future<int> sf = f.share();

	for(int i = 0; i < 10; i++)
	{
		_threads.push_back(std::thread(factorial, sf, i));
	}

	for(auto& thread : _threads )
	{
		if(thread.joinable())
			thread.join();
	}

	p.set_value(4);
	condFuture.notify_all();

	std::this_thread::sleep_for(std::chrono::seconds(30));

	return 0;
}