#include "Atropos.h"

#pragma comment(lib, "legacy_stdio_definitions.lib")

int main(int argc, char** argv)
{
	// Create engine interface
	IEngine* engine = createEngine();

	if (engine->init("startup.cfg"))
	{
		// Start main loop
		engine->run();
	}

	// Destroy engine interface
	destroyEngine(engine);
	return 0;
}