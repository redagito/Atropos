#include "Atropos.h"

#include "engine/Engine.h"

IEngine* createEngine()
{
	return Engine::create();
}

void destroyEngine(IEngine* engine)
{
	delete engine;
}