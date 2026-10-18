#pragma once

#include <cassert>
#include <type_traits>

template <typename T>
class TSingletonMixin
{
public:
	TSingletonMixin()
	{
		assert(d_instance == nullptr);
		d_instance = static_cast<T*>(this);
	}

	~TSingletonMixin()
	{
		d_instance = nullptr;
		return;
	}

	static T& getInstance()
	{
		return *d_instance;
	}

private:
	static T* d_instance;
};

template <typename T>
T* TSingletonMixin<T>::d_instance = nullptr;
