#pragma once

#include <string>

class IResource
{
public:
	virtual ~IResource();

	/**
	* \brief Loads the resource; throws std::runtime_error on failure.
	*/
	virtual void load(const std::string& file) = 0;
};
