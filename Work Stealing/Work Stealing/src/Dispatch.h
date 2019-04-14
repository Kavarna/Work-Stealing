#pragma once


#include "interfaces/ISingletone.h"
#include "WorkerThread.h"
#include <vector>



class Dispatch : public Singletone<Dispatch>
{
	using ThreadPtr = std::unique_ptr<WorkerThread>;
public:
	Dispatch();
	Dispatch(uint32_t numThreads);

public:
	void GiveTask(Task&& task);
	void GiveTask(const decltype(Task::m_task)& task, Task::FlagType);

private:
	void AllocateThreads(uint32_t numThreads);
	std::optional<Task> Steal(uint32_t threadIndex);

private:
	uint32_t							m_numThreads;
	std::vector<ThreadPtr>				m_workerThreads;
};
