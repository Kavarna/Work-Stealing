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
	std::cout << buffer;
	std::this_thread::sleep_for(std::chrono::nanoseconds(50));
}

int main()
{
	auto dispatch = Dispatch::Get();

	for (int i = 0; i < MAX_SIZE; ++i)
	{
		dispatch->GiveTask(std::bind(Task, i), -1);
	}

	std::this_thread::sleep_for(std::chrono::seconds(2));
}