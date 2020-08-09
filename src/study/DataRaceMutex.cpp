#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <fstream>

std::mutex mu;

void shared_print(std::string msg, int id)
{
	std::lock_guard<std::mutex> guard(mu);
	//mu.lock();
	std::cout << msg << id << std::endl;
	//mu.unlock();
}

class Fctor
{
public:
	void operator()()
	{
		for(int i = 0; i>-100; i--)
			shared_print("From Fctor: ", i );
	}
};

class LogFile
{
	std::mutex m_mutex;
	std::ofstream f;
public:
	LogFile()
	{
		f.open("log.txt");
	}

	~LogFile()
	{
		if(f.is_open())
			f.close();
	}

	void shared_print(std::string msg, int id)
	{
		std::lock_guard<std::mutex> guard(m_mutex);
		f << msg << id << std::endl;
	}
};

/*
class stack
{
	int* _data;
	std::mutex _mu;
public:
	//void pop(); // No thread safe
	//int& top(); // No thread safe

	int& pop();
};

void functio_2(stack& st)
{
	{ // No thread safe
	//int v = st.top();
	//st.pop();
	}
	int v = st.pop();
	//process(v);
}

*/
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