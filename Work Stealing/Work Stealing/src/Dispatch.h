#pragma once


#include "interfaces/ISingletone.h"
#include "SetMonitor.h"
#include "WorkerThread.h"
#include "Task.h"
#include <vector>
#include <unordered_set>



class Dispatch : public Singletone<Dispatch>
{
	using ThreadPtr = std::unique_ptr<WorkerThread>;
public:
	Dispatch();
	Dispatch(uint32_t numThreads);

public:
	int selectBestThread();
	void GiveTask(Task&& task);
	void GiveTask(const decltype(Task::m_task)& task, Task::FlagType);
	
	void WaitAll();
	void Wait(const Task::FlagType flag);

private:
	void AllocateThreads(uint32_t numThreads);
	std::optional<Task> Steal(uint32_t threadIndex);
	void FinishTask(Task::FlagType flag);

private:
	uint32_t							m_numThreads;
	std::vector<ThreadPtr>				m_workerThreads;

	std::mutex							m_threadMutex;
	std::condition_variable				m_condVariable;

	//std::unordered_set<Task::FlagType>	m_activeKeys;
	SetMonitor<Task::FlagType>			m_activeKeys;
	// TODO: Implement a queue for main thread, so it won't sleep too much
};
