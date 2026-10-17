#include "engine/Engine.h"

// Sub systems
#include "renderer/RenderSystem.h"
#include "collision/CollisionSystem.h"
#include "sound/SoundSystem.h"
#include "game/GameSystem.h"

// Game states
#include "game/state/LoadState.h"
#include "game/state/LoseState.h"
#include "game/state/WinState.h"
#include "game/state/PauseState.h"

// Debug and profiling
#include "debug/Profiler.h"

// Worker pool
#include "core/ThreadPool.h"

// IO and utility
#include "io/ConfigFile.h"
#include "util/Global.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <memory>

#include "glfw/glfw3.h"

Engine::Engine()
	:
	d_collisionSystem(nullptr),
	d_gameSystem(nullptr),
	d_renderSystem(nullptr),
	d_soundSystem(nullptr),
	d_init(false),
	d_running(false)
{
	return;
}

Engine::~Engine()
{
	cleanup();
	// Log profiler data
	SPDLOG_INFO("{}", Profiler::toString());
}

void Engine::init(const char* configFile)
{
	// Prevent multiple init calls
	if (d_init)
	{
		return;
	}

	// Configuration file with startup config
	ConfigFile config;
	config.load(configFile);

	// Set up console and file logging before creating any subsystem.
	auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
		config.get("log_filename", std::string("log.txt")), true);
	auto logger = std::make_shared<spdlog::logger>(
		"atropos", spdlog::sinks_init_list{ consoleSink, fileSink });
	logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
	logger->set_level(spdlog::level::trace);
	spdlog::set_default_logger(logger);

	// Worker pool used by the resource loaders. It must outlive every system
	// that owns a loader.
	const int workerCount = config.get("engine_workerthreads", 4);
	d_pool = std::make_unique<ThreadPool>(static_cast<unsigned int>(workerCount < 1 ? 1 : workerCount));
	SPDLOG_INFO("Created worker pool with {} threads.", d_pool->getWorkerCount());

	// Initialize rendering system singleton
	SPDLOG_INFO("Create rendering system.");
	d_renderSystem.reset(new RenderSystem);
	d_renderSystem->init(config, *d_pool);

	// Initialize sound system
	SPDLOG_INFO("Create sound system.");
	d_soundSystem.reset(SoundSystem::create());
	d_soundSystem->init(config, *d_pool);

	SPDLOG_INFO("Create collision system.");
	// Initialize collision system
	// TODO Should be physics system
	d_collisionSystem.reset(new CollisionSystem());

	// Create game system
	SPDLOG_INFO("Create game system.");
	d_gameSystem.reset(GameSystem::create());

	// Create game states
	// TODO This should be data driven
	SPDLOG_INFO("Create game states.");
	createGameStates();

	// Init game system
	d_gameSystem->init(config); // Init subsystem

	// Init successful
	d_init = true;
}

void Engine::cleanup()
{
	d_init = false;

	// Delete game system first
	d_gameSystem = nullptr;

	// Stop the workers before destroying the systems that own the loaders,
	// so no in flight parse can touch a destroyed loader.
	d_pool = nullptr;

	// Delete sound system.
	d_soundSystem = nullptr;
		
	// Delete render system
	d_renderSystem = nullptr;

	// Collision system deleted last
	d_collisionSystem = nullptr;
}

void Engine::run()
{
	SPDLOG_INFO("Starting game engine main loop.");
	// Time difference
	float dtime = 0.f;
	float f2Reset = 0.f;

	// Main loop
	d_running = true;

	while (!glfwWindowShouldClose(Global::window) && d_running)
	{
		Profiler loopProfiler("engine_loop");
		// Set start time
		float startTime = (float)glfwGetTime();

		// Process user input
		glfwPollEvents();

		// F2 swaps culling
		if (glfwGetKey(Global::window, GLFW_KEY_F2) && f2Reset > 0.5f)
		{
			f2Reset = 0.f;
			d_renderSystem->setCulling(!d_renderSystem->getCulling());
		}

		// ESC quits application
		if (glfwGetKey(Global::window, GLFW_KEY_ESCAPE))
		{
			// Termination
			glfwSetWindowShouldClose(Global::window, true);
		}
		
		f2Reset += dtime;

		updateResources();
		updateRenderer();
		updateGame(dtime);
		updateCollision();
		swapBuffer();

		// Update time diff
		dtime = (float) glfwGetTime() - startTime;
	}
}

void Engine::createGameStates()
{
	// Add game states
	// Load state, is initial state
	d_gameSystem->addState("load", new LoadState(d_renderSystem.get(), d_collisionSystem.get(), d_soundSystem.get(), d_gameSystem.get()));
	// Win state
	d_gameSystem->addState("win", new WinState(d_renderSystem.get(), d_collisionSystem.get(), d_soundSystem.get()));
	// Lose state
	d_gameSystem->addState("lose", new LoseState(d_renderSystem.get(), d_collisionSystem.get(), d_soundSystem.get()));
	// Pause state
	d_gameSystem->addState("pause", new PauseState(d_renderSystem.get(), d_collisionSystem.get(), d_soundSystem.get()));
}

void Engine::updateGame(float dtime)
{
	// Update game logic
	Profiler profiler("update_game");
	d_running = d_gameSystem->update(dtime);
}

void Engine::updateCollision()
{
	// Update collisions
	Profiler profiler("update_collision");
	d_collisionSystem->update();
}

void Engine::updateResources()
{
	// Commit finished resources on the main thread
	Profiler profiler("update_resources");
	d_renderSystem->process();
	d_soundSystem->process();
}

void Engine::updateRenderer()
{
	// Draw the scene
	Profiler profiler("update_renderer");
	d_renderSystem->draw();
}

void Engine::swapBuffer()
{
	// Finish draw and swap buffer
	Profiler profiler("swap_buffer");
	d_renderSystem->swapBuffer();
}
