#pragma once

/**
* \brief Module interface class.
* A module is a dynamically loadable sub-system, which performs a specific task.
*/
class IModule
{
public:
	virtual ~IModule();

	/**
	* \brief Initialization.
	* \return True on success and false on failure.
	* Called after the module was loaded and the object was created.
	*/
	virtual bool init() = 0;

	/**
	* \brief Deinitialization.
	* Frees module resources. Called on module unload.
	*/
	virtual void cleanup() = 0;
};