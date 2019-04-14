#include "Dispatch.h"

Dispatch::Dispatch()
{
	AllocateThreads(std::thread::hardware_concurrency());
}

Dispatch::Dispatch(uint32_t numThreads)
{
	AllocateThreads(numThreads);
}

void Dispatch::AllocateThreads(uint32_t numThreads)
{
	m_numThreads = numThreads;
	m_workerThreads.resize(numThreads);
	for (uint32_t i = 0; i < numThreads; ++i)
	{
		m_workerThreads[i] = std::make_unique<WorkerThread>(std::bind(&Dispatch::Steal, this, i));
	}
}

std::optional<Task> Dispatch::Steal(uint32_t threadIndex)
{
	for (uint32_t i = 1; i < m_numThreads; ++i)
	{
		int index = (i + threadIndex) % m_numThreads;
		auto value = m_workerThreads[index]->Steal();
		if (value.has_value())
			return value;
	}
	return std::nullopt;
}

void Dispatch::GiveTask(Task&& task)
{
	int bestThread = 0;
	int maxTasks = INT_MAX;
	for (uint32_t i = 0; i < m_numThreads - 1; ++i)
	{
		int numTasks = m_workerThreads[i]->getNumTasks();
		if (numTasks > maxTasks)
		{
			maxTasks = numTasks;
		}
	}
	m_workerThreads[bestThread]->GiveTask(std::move(task));
}

void Dispatch::GiveTask(const decltype(Task::m_task)& task, Task::FlagType flag)
{
	int bestThread = 0;
	int maxTasks = INT_MAX;
	for (uint32_t i = 0; i < m_numThreads - 1; ++i)
	{
		int numTasks = m_workerThreads[i]->getNumTasks();
		if (numTasks > maxTasks)
		{
			maxTasks = numTasks;
		}
	}
	m_workerThreads[bestThread]->GiveTask(std::move(Task{task, flag}));
}
