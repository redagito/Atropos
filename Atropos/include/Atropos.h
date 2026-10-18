#pragma once

#include "engine/IEngine.h"

/**
* TODO: Create engine as dynamic lib instead of static.
*/
extern "C"
{
	IEngine* createEngine();

	void destroyEngine(IEngine*);
}