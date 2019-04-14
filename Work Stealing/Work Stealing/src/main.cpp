#include "Dispatch.h"
#include <iostream>

void Task(int arg)
{
	std::cout << "Hello from " << arg << "\n";
}

int main()
{
	auto dispatch = Dispatch::Get(1);

	for (int i = 0; i < 100; ++i)
	{
		dispatch->GiveTask(std::bind(Task, i), -1);
	}


	std::this_thread::sleep_for(std::chrono::seconds(60));
}