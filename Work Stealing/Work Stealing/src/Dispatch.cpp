#include "Dispatch.h"


#include "common.h"

Dispatch::Dispatch()
{
	AllocateThreads(std::thread::hardware_concurrency() - 1); // Main thread is also a thread
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
		m_workerThreads[i] = std::make_unique<WorkerThread>(std::bind(&Dispatch::Steal, this, i),
			std::bind(&Dispatch::FinishTask, this, std::placeholders::_1));
	}
	for (uint32_t i = 0; i < numThreads; ++i)
	{
		m_workerThreads[i]->Start();
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

void Dispatch::FinishTask(Task::FlagType flag)
{
	// TODO: Make this thread-safe
	// m_activeKeys.erase(flag);
	// m_condVariable.notify_one();
}

int Dispatch::selectBestThread()
{
	int bestThread = 0;
	int maxTasks = INT_MAX;
	for (uint32_t i = 0; i < m_numThreads - 1; ++i)
	{
		int numTasks = m_workerThreads[i]->getNumTasks();
		if (numTasks < maxTasks)
		{
			maxTasks = numTasks;
			bestThread = i;
		}
	}
	return bestThread;
}

void Dispatch::GiveTask(Task&& task)
{
	m_activeKeys.insert(task.flag);
	int bestThread = selectBestThread();

	if (!m_workerThreads[bestThread]->GiveTask(std::move(task)))
	{
		task.m_task();
		m_activeKeys.erase(task.flag);
	}
}

void Dispatch::GiveTask(const decltype(Task::m_task)& task, Task::FlagType flag)
{
	GiveTask(std::move(Task{ task, flag }));
}

void Dispatch::WaitAll()
{
}

void Dispatch::Wait(const Task::FlagType flag)
{
}
