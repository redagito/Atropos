#include "CDynamicLibrary.h"

CDynamicLibrary::CDynamicLibrary()
	:
	d_hModule(NULL)
{
	return;
}

CDynamicLibrary::~CDynamicLibrary()
{
	unload();
}

bool CDynamicLibrary::load(const std::string& fileName)
{
	unload();
	d_hModule = LoadLibraryA(fileName.c_str());
	if (d_hModule == NULL)
	{
		return false;
	}
	return true;
}

void CDynamicLibrary::unload()
{
	if (d_hModule != NULL)
	{
		FreeLibrary(d_hModule);
		d_hModule = NULL;
	}
}

void* CDynamicLibrary::getFunction(const std::string& name)
{
	if (d_hModule == NULL)
	{
		return nullptr;
	}
	return (void*)GetProcAddress(d_hModule, name.c_str());
}
