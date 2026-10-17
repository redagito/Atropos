#pragma once

#include <string>

/**
* \brief Resource interface class.
* A resource represents loadable data.
*/
class IResource
{
public:
	/**
	* \brief Virtual destructor for interface class.
	*/
	virtual ~IResource();

	/**
	* \brief Loads resource from file.
	* Throws std::runtime_error on failure.
	*/
	virtual void load(const std::string& file) = 0;
};
