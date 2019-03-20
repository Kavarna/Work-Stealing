#pragma once


#include <functional>


struct Task
{
	typedef uint64_t FlagType;
	FlagType triggerFlag;

	std::function<void()> m_task;
};