#include "Atropos.h"

#include "engine/CEngine.h"

IEngine* createEngine()
{
	return CEngine::create();
}

void destroyEngine(IEngine* engine)
{
	delete engine;
}