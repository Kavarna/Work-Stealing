#pragma once

#include <thread>
#include <atomic>
#include <deque>
#include <optional>
#include <mutex>
#include <condition_variable>

#include "Task.h"

#if defined _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

class WorkerThread
{
public:
	WorkerThread() = delete;
	explicit WorkerThread(std::function<std::optional<Task>()> stealFunc,
		std::function<void(Task::FlagType)> triggerFlag);
	WorkerThread(const WorkerThread&&) = delete;
	WorkerThread(const WorkerThread&) = delete;
	const WorkerThread& operator=(const WorkerThread&) = delete;
	~WorkerThread();

public:
	void GiveTask(const Task& task);

	std::optional<Task> Steal();

	uint32_t getNumTasks() { return m_nTasks; };

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
	std::function<void(Task::FlagType)>			m_triggerFlag = nullptr;

	std::deque<Task>							m_allTasks;
	std::atomic<uint32_t>						m_nTasks;
};