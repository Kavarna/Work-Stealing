#pragma once

#include <functional>

struct Task
{
	typedef uint64_t FlagType;
	std::function<void()> m_task;
	FlagType flag;
};
