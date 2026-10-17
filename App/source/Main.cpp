#include "Atropos.h"

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