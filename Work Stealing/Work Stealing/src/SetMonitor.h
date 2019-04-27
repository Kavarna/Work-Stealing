#pragma once


#include <unordered_set>
#include <mutex>


template <class KeyType>
class SetMonitor
{
public:
	SetMonitor() = default;
	/// <summary>Constructs a SetMonitor from an initializer_list. !NOT THREAD-SAFE</summary>
	SetMonitor(const std::initializer_list<KeyType>& list)
	{
		for (const auto& it : list)
		{
			m_set.insert(it);
		}
	}
	/// <summary>Constructs a SetMonitor from an unordered_set. !NOT THREAD-SAFE</sumarry>
	SetMonitor(const std::unordered_set<KeyType>& set) :
		m_set(set)
	{ };

	/// <summary>Copy Constructor</summary>
	SetMonitor(const SetMonitor& rhs) :
		m_set(rhs.m_set)
	{ };

	void Insert(const KeyType& key)
	{
		std::unique_lock<std::mutex> locker(m_locker);
		m_set.insert(key);
	}

	void Delete(const KeyType& key)
	{
		std::unique_lock<std::mutex> locker(m_locker);
		if (auto keyIt = m_set.find(key); keyIt != m_set.end())
			m_set.erase(keyIt);
	}

	bool Empty() const
	{
		return m_set.empty();
	}

	bool HasValue(const KeyType& key) const
	{
		return m_set.find(key) != m_set.end();
	}

private:
	std::unordered_set<KeyType>		m_set;
	std::mutex						m_locker;

};