#pragma once

#include <thread>
#include <atomic>
#include <optional>
#include <mutex>
#include <condition_variable>

#include "concurrentqueue.h"

#if defined _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

struct Task
{
	typedef uint64_t FlagType;
	std::function<void()> m_task;
	FlagType flag;
};

class WorkerThread
{
public:
	WorkerThread() = delete;
	explicit WorkerThread(std::function<std::optional<Task>()> stealFunc);
	WorkerThread(const WorkerThread&&) = delete;
	WorkerThread(const WorkerThread&) = delete;
	const WorkerThread& operator=(const WorkerThread&) = delete;
	~WorkerThread();

public:
	void GiveTask(Task&& task);
	void GiveTask(const decltype(Task::m_task)& task, Task::FlagType);

	// TODO: Implement
	std::optional<Task> Steal();
	uint32_t getNumTasks() const { return m_allTasks.size_approx(); };

	void Wait();
	void StartAfterWait();

private:
	std::optional<Task> GetTask();
	std::optional<Task> Pop();

private:
	void Run();

private:
	std::thread									m_thread;
	std::mutex									m_threadMutex;
	std::condition_variable						m_conditionVariable;

	
	bool										m_wait = false;
	bool										m_running = false;

	std::function<std::optional<Task>()>		m_stealFunction = nullptr;

	moodycamel::ConcurrentQueue<Task>			m_allTasks;
};