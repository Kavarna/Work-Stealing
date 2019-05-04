#include "Dispatch.h"
#include <iostream>
#include <sstream>
#include <string>

#define MAX_SIZE 10000
bool tasks[MAX_SIZE] = {};

void Task(int arg)
{
	tasks[arg] = true;
	char buffer[255];
	sprintf_s(buffer, "Simulating work %d from thread %d\n", arg, std::this_thread::get_id());
	std::cout << buffer;
	//std::this_thread::sleep_for(std::chrono::nanoseconds(50));
	int i = 0;
	while (i++ < MAX_SIZE);
}

#if DEBUG || _DEBUG
void DumpData()
{
	std::this_thread::sleep_for(std::chrono::seconds(10));
	Dispatch::Get()->DumpData();
}
#endif


int main()
{
	auto dispatch = Dispatch::Get();


	//std::thread th(DumpData);
	for (int i = 0; i < MAX_SIZE; ++i)
	{
		dispatch->GiveTask(std::bind(Task, i), i);
		dispatch->Wait(i);
		if (i % 100 == 0)
			dispatch->WaitAll();
	}

	dispatch->WaitAll();
	//th.join();
	
	for (int i = 0; i < MAX_SIZE; ++i)
	{
		if (tasks[i] == false)
		{
			std::cout << "Oooooopsie\n";
			return 1;
		}
	}
	return 0;
}