#pragma once


#include <thread>
#include <atomic>
#include <optional>
#include <mutex>
#include <condition_variable>

#include "concurrentqueue.h"
#include "Task.h"


class IWorker
{
public:
	IWorker() : m_allTasks(300)
	{ };

	virtual bool GiveTask(Task&& task) = 0;
	virtual bool GiveTask(const decltype(Task::m_task)& task, Task::FlagType) = 0;

	virtual std::optional<Task> Pop() = 0;
	virtual std::optional<Task> Steal() = 0;
	virtual uint32_t getNumTasks() const { return (uint32_t)m_allTasks.size_approx(); };

protected:
	moodycamel::ConcurrentQueue<Task>			m_allTasks;
};