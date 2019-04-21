#include "WorkerThread.h"

WorkerThread::WorkerThread(std::function<std::optional<Task>()> stealFunc,
	std::function<void(Task::FlagType)> callbackWhenDone) :
	m_stealFunction(stealFunc),
	m_callbackWhenDone(callbackWhenDone)
{
	m_running = true;
}

WorkerThread::~WorkerThread()
{
	m_running = false;
#if defined _WIN32
	HANDLE threadHandle = m_thread.native_handle();
	TerminateThread(threadHandle, 0);
#endif

	m_thread.detach();
}

void WorkerThread::Start()
{
	m_thread = std::thread(&WorkerThread::Run, this);
}

bool WorkerThread::GiveTask(Task&& task)
{
	bool result = m_allTasks.try_enqueue(task);
	if (!result)
	{
		return false;
	}
	m_conditionVariable.notify_one();
	return true;
}

bool WorkerThread::GiveTask(const decltype(Task::m_task)& task, Task::FlagType flag)
{
	bool result = m_allTasks.try_enqueue(Task{ task, flag });
	if (!result)
		return false;

	m_conditionVariable.notify_one();
	return true;
}

std::optional<Task> WorkerThread::Steal()
{
	return Pop(); // With this slick queue, Steal() and Pop() can be the same
}

void WorkerThread::Wait()
{
	m_wait = true;
}

void WorkerThread::StartAfterWait()
{
	m_wait = false;
	m_conditionVariable.notify_one();
}

std::optional<Task> WorkerThread::GetTask()
{
	if (m_wait)
		return std::nullopt;
	auto myTask = Pop();
	if (myTask.has_value())
		return myTask;

	return m_stealFunction();
}

std::optional<Task> WorkerThread::Pop()
{
	bool found;
	Task task;
	found = m_allTasks.try_dequeue(task);
	if (found)
	{
		return task;
	}
	return std::nullopt;
}

void WorkerThread::Run()
{
	std::unique_lock<std::mutex> locker(m_threadMutex);
	while (true)
	{
		auto task = GetTask();
		while (!task.has_value())
		{
			m_conditionVariable.wait(locker, [&] { task = GetTask(); return task.has_value(); });
		}

		(*task).m_task();
		m_callbackWhenDone((*task).flag);
	}
}
