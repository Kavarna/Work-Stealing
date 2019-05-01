#include "Synchronization.h"

std::shared_mutex g_mutex;
std::condition_variable_any g_conditionVariable;