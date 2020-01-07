#pragma once

#include "IResource.h"

#include <string>

#include <Windows.h>

/**
* \brief Dynamic library class.
*/
class CDynamicLibrary : public IResource
{
public:
	/**
	* \brief Sets default values.
	*/
	CDynamicLibrary();

	/**
	* \brief Frees dynamic library resource.
	*/
	~CDynamicLibrary();

	/**
	* \brief Loads dynamic library from file.
	*/
	bool load(const std::string& file);

	/**
	* \brief Unloads dynamic library.
	*/
	void unload();

	/**
	* \brief Returns function pointer.
	*/
	void* getFunction(const std::string& name);
	
private:
	HMODULE d_hModule; /**< DLL module handle. */
};