#pragma once

#include <thread>
#include <atomic>
#include <optional>
#include <condition_variable>

#include "Task.h"
#include "interfaces/IWorker.h"
#include "Synchronization.h"

#if defined _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#if DEBUG || _DEBUG
#include <fstream>
#endif

class WorkerThread : public IWorker
{
public:
	WorkerThread() = delete;
	explicit WorkerThread(std::function<std::optional<Task>()> stealFunc,
		std::function<void(Task::FlagType)> callbackWhenDone);
	WorkerThread(const WorkerThread&&) = delete;
	WorkerThread(const WorkerThread&) = delete;
	const WorkerThread& operator=(const WorkerThread&) = delete;
	~WorkerThread();

public:
	void Start();

	bool GiveTask(Task&& task) override;
	bool GiveTask(const decltype(Task::m_task)& task, Task::FlagType) override;

	std::optional<Task> Steal();
	uint32_t getNumTasks() const { return (uint32_t)m_allTasks.size_approx(); };

	void Wait();
	void StartAfterWait();

#if DEBUG || _DEBUG
	void DumpData(const std::string& filePath)
	{
		std::ofstream fout(filePath);

		auto oldSize = m_allTasks.size_approx();
		auto size = m_allTasks.size_approx();
		int times = 0;
		while (oldSize != size)
		{
			oldSize = size;
			size = m_allTasks.size_approx();
			times++;
		}

		fout << "Thread ID = " << std::this_thread::get_id() << "; No. Tasks = " << size << "; No. of Tries = " << times << ";\n";
		fout.close();
	}
#endif

private:
	std::optional<Task> GetTask();
	std::optional<Task> Pop();

private:
	void Run();

private:
	std::thread									m_thread;

	
	bool										m_wait = false;
	bool										m_running = false;

	std::function<std::optional<Task>()>		m_stealFunction = nullptr;
	std::function<void(Task::FlagType)>			m_callbackWhenDone = nullptr;
};