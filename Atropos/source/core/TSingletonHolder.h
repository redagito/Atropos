#pragma once

#include <memory>
#include <cassert>

template <typename T>
class TSingletonHolder
{
public:
	TSingletonHolder()
	{
		assert(s_instance == nullptr);
	}

	void setInstance(T* instance)
	{
		assert(s_instance == nullptr);
		s_instance = instance;
	}

	void deleteInstance()
	{
		s_instance = nullptr;
	}

	static T& getInstance()
	{
		assert(s_instance != nullptr);
		return *s_instance;
	}

	~TSingletonHolder()
	{
		deleteInstance();
	}

private:
	static std::unique_ptr<T> s_instance;
};

template <typename T>
std::unique_ptr<T> TSingletonHolder<T>::s_instance = nullptr;
