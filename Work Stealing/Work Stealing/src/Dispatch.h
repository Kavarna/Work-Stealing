#pragma once


#include "interfaces/ISingletone.h"
#include "TasksMonitor.h"
#include "WorkerThread.h"
#include "Task.h"
#include "interfaces/IWorker.h"
#include <vector>
#include <unordered_set>
#include <sstream>


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

#if DEBUG || _DEBUG
	void DumpData()
	{
		for (uint32_t i = 0; i < m_workerThreads.size(); ++i)
		{
			m_workerThreads[i]->DumpData(std::to_string(i));
		}

		m_activeKeys.DumpData();
	}
#endif

private:
	void AllocateThreads(uint32_t numThreads);
	std::optional<Task> Steal(uint32_t threadIndex);
	void FinishTask(Task::FlagType flag);

private:
	uint32_t							m_numThreads;
	std::vector<ThreadPtr>				m_workerThreads;

	TasksMonitor<Task::FlagType>		m_activeKeys;
	// TODO: Implement a queue for main thread, so it won't sleep too much
};
