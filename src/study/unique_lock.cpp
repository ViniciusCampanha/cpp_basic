#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <fstream>

class LogFile
{
	std::mutex _mu;
	std::once_flag _flag;
	std::mutex _mu_open;
	std::ofstream f;
public:
	LogFile() { }

	~LogFile()
	{
		if(f.is_open())
			f.close();
	}

	void shared_print(std::string msg, int id)
	{
		/*
		std::unique_lock<std::mutex> locker2(_mu_open);
		if(!f.is_open())
		{			
			f.open("log.txt");
		}*/
		std::call_once(_flag, 
			[&](){f.open("log.txt");});

		std::unique_lock<std::mutex> locker(_mu, std::defer_lock);
		// do something else

		locker.lock();
		f << msg << id << std::endl;
		locker.unlock();

		locker.lock();
		// do other things
		locker.unlock();

		std::unique_lock<std::mutex> locker2 = std::move(locker);
	}
};

void functio_1(int& value, LogFile& log)
{
	for(; value>-100; value--)
		log.shared_print("From t1: ", value );
}

int main()
{
	LogFile log;
	int value = 0;
	std::thread t1 (functio_1, std::ref(value), std::ref(log)); // starts running

	for(int i = 0; i<100; i++)
		log.shared_print("From main: ", i);
	
	if(t1.joinable())
		t1.join();

	return 0;
}