#pragma once

class IModule;

#include <string>

/**
 * \brief Module manager interface class.
 * Loads and manages lifetime of module objects.
 */
class IModuleManager
{
	/**
	 * \brief Virtual destructor.
	 */
	virtual ~IModuleManager();

	/**
	* \brief Loads module by name.
	* \return True on success and false otherwise.
	*/
	virtual bool loadModule(const std::string& moduleName) = 0;

	/**
	 * \brief Returns load state of the specified module.
	 * \return True if the specified module is loaded and false otherwise.
	 */
	virtual bool hasModule(const std::string& moduleName) const = 0;

	/**
	* \brief Returns module interface object.
	* \brief return Pointer to module interface or nullptr on error.
	* If the module is not loaded, loadModule will be called with the module name
	* as argument. If the module could not be loaded a nullptr will be returned.
	*/
	virtual IModule* getModule(const std::string& moduleName) const = 0;
};
