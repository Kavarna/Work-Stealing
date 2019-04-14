#pragma once


#include <exception>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <chrono>
#include <functional>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <optional>


#if defined DEBUG || _DEBUG
#include <iostream>
#else
#endif

#pragma region appendToString()

template <typename ... Args>
constexpr auto appendToString() -> std::string
{
	return "";
}

template <typename type>
constexpr auto appendToString(type arg) -> std::string
{
	std::ostringstream stream;
	stream << arg;
	return stream.str();
}

template <typename type, typename... Args>
constexpr auto appendToString(type A, Args... args) -> std::string
{
	std::string res1 = appendToString(A);
	std::string res2 = appendToString(args...);
	return res1 + res2;
}

template <typename type>
constexpr auto putSetInVector(const std::set<type>& set) -> std::vector<type>
{
	std::vector<type> ret;

	for (const auto& e : set)
		ret.push_back(e);

	return ret;
}

#pragma endregion

#pragma region PrintToConsole()

	template <typename ... Args>
	constexpr auto printToConsole()
	{ }

	template <typename type>
	constexpr auto printToConsole(type arg)
	{
#if DEBUG || _DEBUG
		std::cout << arg;
#endif
	}

	template <typename type, typename... Args>
	constexpr auto printToConsole(type A, Args... args)
	{
#if DEBUG || _DEBUG
		std::cout << A;
		printToConsole(args...);
#endif
	}

	template <bool flush, typename type, typename... Args>
	constexpr auto printToConsole(type A, Args... args)
	{
#if DEBUG || _DEBUG
		std::cout << A;
		printToConsole(args...);
		if constexpr (flush)
		{
			std::cout.flush();
		}
#endif
	}

#pragma endregion
