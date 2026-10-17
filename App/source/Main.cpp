#include "Atropos.h"

#include <spdlog/spdlog.h>

#include <exception>
#include <iostream>
#include <memory>

namespace
{
	struct EngineDeleter
	{
		void operator()(IEngine* engine) const
		{
			destroyEngine(engine);
		}
	};
}

int main()
{
	// Owns the engine, so it is destroyed even if initialization or the main
	// loop throws.
	std::unique_ptr<IEngine, EngineDeleter> engine(createEngine());

	try
	{
		engine->init("startup.cfg");
		engine->run();
	}
	catch (const std::exception& e)
	{
		SPDLOG_CRITICAL("{}", e.what());
		std::cerr << "Fatal error: " << e.what() << std::endl;
		return 1;
	}
	catch (...)
	{
		SPDLOG_CRITICAL("Unknown fatal error.");
		std::cerr << "Fatal error: unknown exception." << std::endl;
		return 1;
	}

	return 0;
}
