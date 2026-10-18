#pragma once

#include <type_traits>

template <typename Derived, typename Base = Derived>
class TCreatorMixin
{
public:
	virtual ~TCreatorMixin()
	{
		return;
	}

	static Base* create()
	{
		return new Derived;
	}
};
