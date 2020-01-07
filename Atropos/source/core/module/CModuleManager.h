#pragma once

class IModule;
class CDynamicLibrary;

#include <string>
#include <unordered_map>
#include <memory>

/**
* \brief Dll based module loader and manager.
* The dll must provide 2 functions: a module creator with signature IModule* createModule(void)
* and a module destroyer with signature void destroyModule(IModule*).
* On load, the dll is loaded, the creator and destroyer function pointers are set and the creator
* function is called. The module object is stored and can be accessed with the getModule function.
* Due to this functionality, only one module can be created from a dll at a time.
*/
class CModuleManager
{
public:
	/**
	* \brief Frees all modules.
	*/
	~CModuleManager();

	/**
	* \brief Loads module file and creates and returns module interface object.
	* \return Pointer to module object, null if failed to load.
	*/
	bool loadModule(const std::string& moduleName);

	/**
	 * \brief Returns true, if the specified module is loaded and false otherwise.
	 */
	bool hasModule(const std::string& moduleName) const;

	/**
	* \brief Returns loaded module interface object.
	* \return Loaded module interface or null if the module has not been loaded.
	*/
	IModule* getModule(const std::string& moduleName);

private:
	typedef IModule* (*CreatorFunc)(); /**< Module creator function prototype. */
	typedef void (*DestroyerFunc)(IModule*); /**< Module destructor function prototype. */

	/**
	* \brief Module container.
	*/
	struct SModule
	{
		IModule* d_module; /**< Module interface. */
		CreatorFunc d_creator; /**< Module creator. */
		DestroyerFunc d_destroyer; /**< Module destroyer. */
		std::shared_ptr<CDynamicLibrary> d_dynamicLib; /**< Dynamic library resource. */
	};

	std::unordered_map<std::string, SModule> d_modules; /**< Loaded modules. */
};
