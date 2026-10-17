#include "Atropos.h"

int main()
{
	// Create engine interface
	IEngine* engine = createEngine();

	const bool initialized = engine->init("startup.cfg");
	if (initialized)
	{
		// Start main loop
		engine->run();
	}

	// Destroy engine interface
	destroyEngine(engine);
	return initialized ? 0 : 1;
}