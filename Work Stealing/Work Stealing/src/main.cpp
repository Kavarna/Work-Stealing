#include "WorkerThread.h"
#include <iostream>

void Task(int arg)
{
	std::cout << "Hello from " << arg << "\n";
}

int main()
{
	WorkerThread thread([]
	{
		return std::nullopt;
	});
	for (int i = 0; i < 100; ++i)
	{
		thread.GiveTask(std::bind(Task, i), -1);
	}

	std::this_thread::sleep_for(std::chrono::seconds(50));
}