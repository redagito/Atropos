#include "Atropos.h"

#include <spdlog/sinks/basic_file_sink.h>
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
	// GUI subsystem means no console for stderr or spdlog's stdout sink; the file logger goes first.
	auto bootstrapLogger = spdlog::basic_logger_mt("atropos", "log.txt");
	bootstrapLogger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
	bootstrapLogger->set_level(spdlog::level::trace);
	spdlog::set_default_logger(bootstrapLogger);

	// Owns the engine, so it is destroyed even if init or the main loop throws.
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
