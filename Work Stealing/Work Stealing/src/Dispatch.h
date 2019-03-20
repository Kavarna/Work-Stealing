#pragma once


#include "interfaces/ISingletone.h"
#include <vector>
#include <deque>
#include <unordered_set>
#include "WorkerThread.h"

class Dispatch : public Singletone<Dispatch>
{
	using WorkerThreadPtr = std::unique_ptr<WorkerThread>;
public:
	Dispatch();
	Dispatch(uint32_t numThreads);
	~Dispatch();


	// TODO: Implement this
	void Wait(Task::FlagType flag);
	void TriggerFlag(Task::FlagType flag);

	void Stop();

	std::optional<Task> Steal(uint32_t index);

private:
	void AllocateThreads(uint32_t numThreads);

private:
	std::vector<WorkerThreadPtr> m_workerThreads;

	std::unordered_set<Task::FlagType> m_activeFlags;
};