#pragma once

#include <shared_mutex>

template <typename T>
using Lock = std::shared_lock<T>;

extern std::shared_mutex g_mutex;
extern std::condition_variable_any g_conditionVariable;