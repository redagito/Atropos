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
	* \return True on success and false on failure.
	*/
	virtual bool load(const std::string& file) = 0;
};
