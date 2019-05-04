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
	m_activeKeys.Delete(flag);
	g_conditionVariable.notify_all();
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
	int bestThread = selectBestThread();

#if DEBUG || _DEBUG
	m_activeKeys.Insert(task.flag, bestThread);
#else
	m_activeKeys.Insert(task.flag);
#endif

	if (!m_workerThreads[bestThread]->GiveTask(std::move(task)))
	{
		// TODO: Store it on main thread's queue (when it will be possible)
		task.m_task();
		m_activeKeys.Delete(task.flag);
	}
}

void Dispatch::GiveTask(const decltype(Task::m_task)& task, Task::FlagType flag)
{
	GiveTask(std::move(Task{ task, flag }));
}

void Dispatch::WaitAll()
{
	while (true)
	{
		Lock<decltype(g_mutex)> locker(g_mutex);
		g_conditionVariable.wait_for(locker, std::chrono::nanoseconds(500), [&] { return m_activeKeys.Empty(); });
		g_conditionVariable.notify_all();
		if (m_activeKeys.Empty())
			return;
	}
}

void Dispatch::Wait(const Task::FlagType flag)
{
	while (true)
	{
		Lock<decltype(g_mutex)> locker(g_mutex);
		g_conditionVariable.wait_for(locker, std::chrono::nanoseconds(500), [&] { return !m_activeKeys.HasValue(flag); });
		g_conditionVariable.notify_all();
		if (!m_activeKeys.HasValue(flag))
			return;
	}
}
