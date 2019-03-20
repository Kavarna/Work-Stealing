#include "WorkerThread.h"

WorkerThread::WorkerThread(std::function<std::optional<Task>()> stealFunc,
	std::function<void(Task::FlagType)> triggerFlag):
	m_stealFunction(stealFunc),
	m_triggerFlag(triggerFlag)
{
	m_running = true;

	m_thread = std::thread(&WorkerThread::Run, this);
}

WorkerThread::~WorkerThread()
{
#if defined _WIN32
	HANDLE threadHandle = m_thread.native_handle();
	TerminateThread(threadHandle, 0);
#endif

	m_thread.detach();
}

void WorkerThread::GiveTask(const Task & task)
{
	m_allTasks.push_back(task);
	m_nTasks++;
}

std::optional<Task> WorkerThread::Steal()
{
	if (m_nTasks > 1)
	{
		Task task = m_allTasks.back();
		m_nTasks--;
		return task;
	}
	return std::nullopt;
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
	//return Pop().value_or(m_stealFunction());
	auto myTask = Pop();
	if (myTask.has_value())
		return myTask;

	return m_stealFunction();
}

std::optional<Task> WorkerThread::Pop()
{
	if (m_nTasks > 0)
	{
		Task task = m_allTasks.front();
		m_nTasks--;
		return task;
	}
	return std::nullopt;
}

void WorkerThread::Run()
{
	while (true)
	{
		auto task = GetTask();
		while (!task.has_value())
		{
			std::unique_lock<std::mutex> locker(m_threadMutex);
			m_conditionVariable.wait(locker, [&] { return m_nTasks > 0; });
			task = GetTask();
		}


		(*task).m_task();
		m_triggerFlag((*task).triggerFlag);
	}
}
