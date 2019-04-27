#include "Dispatch.h"
#include <iostream>
#include <sstream>
#include <string>

#define MAX_SIZE 1000
bool tasks[MAX_SIZE] = {};

void Task(int arg)
{
	tasks[arg] = true;
	char buffer[255];
	sprintf_s(buffer, "Simulating work %d from thread %d\n", arg, std::this_thread::get_id());
	//std::this_thread::sleep_for(std::chrono::nanoseconds(50));
	int i = 0;
	while (i++ < MAX_SIZE);
	std::cout << buffer;
}

int main()
{
	auto dispatch = Dispatch::Get();

	for (int i = 0; i < MAX_SIZE / 2; ++i)
	{
		dispatch->GiveTask(std::bind(Task, i), i);
	}

	
	dispatch->Wait(469);
	
	
	for (int i = MAX_SIZE / 2; i < MAX_SIZE; ++i)
	{
		dispatch->GiveTask(std::bind(Task, i), i);
	}
	dispatch->WaitAll();
	
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