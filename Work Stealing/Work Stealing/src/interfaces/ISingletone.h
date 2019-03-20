#pragma once

#include <memory>
#include <mutex>
#include "../meta/CountParameters.h"

template <typename type>
class Singletone
{
public:
	Singletone() {};
	virtual ~Singletone() {};

public:
	template <typename ...Args>
	static type* Get(Args ... args)
	{
		if constexpr (countParameters<Args...>::value == 0)
		{
			if constexpr (std::is_default_constructible<type>::value)
			{
				std::call_once(m_singletoneFlags, [&]
				{
					m_singletoneInstance = std::make_unique<type>(args...);
					m_singletoneInstance->create();
				});
				return m_singletoneInstance.get();
			}
			else
				return m_singletoneInstance.get();
		}
		else
		{
			std::call_once(m_singletoneFlags, [&]
			{
				m_singletoneInstance = std::make_unique<type>(args...);
				m_singletoneInstance->create();
			});
			return m_singletoneInstance.get();
		}
	}

	virtual void create() {}

	virtual void reset()
	{
		m_singletoneInstance.reset();
	};

private:
	static std::unique_ptr<type> m_singletoneInstance;
	static std::once_flag m_singletoneFlags;
};

template <typename type>
std::unique_ptr<type> Singletone<type>::m_singletoneInstance = nullptr;
template <typename type>
std::once_flag Singletone<type>::m_singletoneFlags;