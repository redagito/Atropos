#include "CModuleManager.h"

#include "resource/CDynamicLibrary.h"
#include "debug/Log.h"

CModuleManager::~CModuleManager()
{
	for (auto& module : d_modules)
	{
		// Call destroyFunc on module
		module.second.d_destroyer(module.second.d_module);
	}
}

bool CModuleManager::loadModule(const std::string& name)
{
	// Module name exists
	if (d_modules.count(name) != 0)
	{
		// Already loaded
		return true;
	}

	SModule module;
	module.d_dynamicLib = std::shared_ptr<CDynamicLibrary>(new CDynamicLibrary);
	
	// Load dynamic library
	if (!module.d_dynamicLib->load(name))
	{
		LOG_ERROR("Could not load module %s.", name.c_str());
		return false;
	}

	// Interface creator and destroyer functions
	module.d_creator = (CreatorFunc) module.d_dynamicLib->getFunction("createModule");
	module.d_destroyer = (DestroyerFunc) module.d_dynamicLib->getFunction("destroyModule");
	if (module.d_creator == nullptr || module.d_destroyer == nullptr)
	{
		LOG_ERROR("Could not load creator and/or destroyer function from module %s.", name.c_str());
		return false;
	}
	
	// Create interface
	module.d_module = module.d_creator();
	if (module.d_module == nullptr)
	{
		LOG_ERROR("Could not create module %s.", name.c_str());
		return false;
	}

	// Add module
	d_modules[name] = module;
	return true;
}

bool CModuleManager::hasModule(const std::string& name) const
{
	return d_modules.count(name) != 0;
}

IModule* CModuleManager::getModule(const std::string& name)
{
	const auto& mod = d_modules.find(name);
	if (mod == d_modules.end())
	{
		LOG_DEBUG("The requested module %s is currently not loaded, attempting to load module.", name.c_str());
		if (!loadModule(name))
		{
			return nullptr;
		}
	}
	return d_modules.at(name).d_module;
}
