#include "Dispatch.h"


Dispatch::Dispatch()
{
	AllocateThreads(std::thread::hardware_concurrency() - 1);
}

Dispatch::Dispatch(uint32_t numThreads)
{
	AllocateThreads(numThreads);
}

Dispatch::~Dispatch()
{
	Stop();
}

void Dispatch::Stop()
{
	for (uint32_t i = 0; i < m_workerThreads.size(); ++i)
	{
		m_workerThreads[i].reset();
	}
}

std::optional<Task> Dispatch::Steal(uint32_t index)
{
	uint32_t maxTasks = 0;
	uint32_t bestChoice = 0;
	for (uint32_t i = 0; i < m_workerThreads.size(); ++i)
	{
		uint32_t tasks = m_workerThreads[i]->getNumTasks();
		if (tasks > maxTasks)
		{
			bestChoice = i;
			maxTasks = tasks;
		}
	}

	if (maxTasks > 0 && index != bestChoice)
		return m_workerThreads[bestChoice]->Steal();

	return std::nullopt;
}

void Dispatch::Wait(Task::FlagType flag)
{
}

void Dispatch::TriggerFlag(Task::FlagType flag)
{
}

void Dispatch::AllocateThreads(uint32_t numThreads)
{
	m_workerThreads.reserve(numThreads);
	for (uint32_t i = 0; i < numThreads; ++i)
	{
		std::function<std::optional<Task>()> stealFunction = std::bind(&Dispatch::Steal, this, i);
		std::function<void(Task::FlagType)> trigger = std::bind(&Dispatch::TriggerFlag, this,
			std::placeholders::_1);
		m_workerThreads.emplace_back(std::make_unique<WorkerThread>(
			stealFunction,
			trigger
			)
		);
	}
}
