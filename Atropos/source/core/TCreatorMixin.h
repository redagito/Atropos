#pragma once

#include <type_traits>

/**
* \brief Creator mixin class.
*/
template <typename Derived, typename Base = Derived>
class TCreatorMixin
{
public:
	virtual ~TCreatorMixin()
	{
		return;
	}

	/**
	* \brief Creates object on heap and returns as pointer to base type.
	*/
	static Base* create()
	{
		return new Derived;
	}
};
