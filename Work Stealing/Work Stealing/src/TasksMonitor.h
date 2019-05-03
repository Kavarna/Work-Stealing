#pragma once


#include <unordered_set>
#include <mutex>


#if DEBUG || _DEBUG
#include <Windows.h>
#include <fstream>
#endif


template <class KeyType>
class TasksMonitor
{
	struct SDebugData
	{
#if DEBUG || _DEBUG
		int ThreadID;
#endif
	};

public:
	TasksMonitor() = default;
	/// <summary>Constructs a SetMonitor from an initializer_list. !NOT THREAD-SAFE</summary>
	TasksMonitor(const std::initializer_list<KeyType>& list)
	{
		for (const auto& it : list)
		{
			m_set.insert({ it, {} });
		}
	}

	/// <summary>Copy Constructor</summary>
	TasksMonitor(const TasksMonitor& rhs) :
		m_set(rhs.m_set)
	{ };

	void Insert(const KeyType& key)
	{
		std::unique_lock<std::mutex> locker(m_locker);
#if _TEXT_DEBUG_
		char buffer[500];
		sprintf_s(buffer, "Checked-in %d; Size = %d \n", key, m_set.size());
		OutputDebugString(buffer);
#endif
		m_set.insert({ key, {} });
	}

#if DEBUG || _DEBUG
	void Insert(const KeyType& key, int threadID)
	{
		std::unique_lock<std::mutex> locker(m_locker);
#if _TEXT_DEBUG_
		char buffer[500];
		sprintf_s(buffer, "Checked-in %d; Size = %d \n", key, m_set.size());
		OutputDebugString(buffer);
#endif
		m_set.insert({ key, {threadID} });
	}
#endif

	void Delete(const KeyType& key)
	{
		std::unique_lock<std::mutex> locker(m_locker);
#if _TEXT_DEBUG_
		char buffer[500];
		sprintf_s(buffer, "Checked-out %d; Size = %d \n", key, m_set.size());
		OutputDebugString(buffer);
#endif
		if (auto keyIt = m_set.find(key); keyIt != m_set.end())
			m_set.erase(keyIt);
	}

	bool Empty()
	{
		std::unique_lock<std::mutex> locker(m_locker);
#if _TEXT_DEBUG_
		OutputDebugString("Empty()\n");
#endif
		return m_set.empty();
	}

	bool HasValue(const KeyType& key)
	{
		std::unique_lock<std::mutex> locker(m_locker);
		if (m_set.empty())
			return false;
		return m_set.find(key) != m_set.end();
	}

#if DEBUG || _DEBUG

	__declspec(noinline) void DumpData() const
	{
		std::ofstream fout("Log.txt");
		
		if (m_set.size())
		{
			for (const auto& it : m_set)
			{
				fout << "Task: " << it.first << " on thread " << it.second.ThreadID << "\n";
			}
		}
		else
		{
			fout << "No tasks available\n";
		}
		
		fout.close();
	}

#endif

private:
	std::unordered_map<KeyType, SDebugData>		m_set;
	std::mutex									m_locker;

};