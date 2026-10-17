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
#include "debug/Log.h"
#include "debug/Logger.h"
#include "debug/Profiler.h"

// IO and utility
#include "io/ConfigFile.h"
#include "util/TimeStamp.h"
#include "util/Global.h"

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
	LOG_INFO("%s", Profiler::toString().c_str());
}

bool Engine::init(const char* configFile)
{
	// Prevent multiple init calls
	if (d_init)
	{
		return true;
	}

	// Configuration file with startup config
	ConfigFile config;
	bool configLoaded = config.load(configFile);

	// Init log file before checking config load state
	Logger::initLogFile(config.get("log_filename", std::string("log/") + TimeStamp::createTimeStamp() + ".log"));

	// Log file initialized, check config load state
	if (!configLoaded)
	{
		LOG_ERROR("Config file %s could not be opened.", configFile);
		// TODO Just proceed with default config values?
		cleanup();
		return false;
	}

	// Initialize rendering system singleton
	LOG_INFO("Create rendering system.");
	d_renderSystem.reset(new RenderSystem);
	if (!d_renderSystem->init(config))
	{
		LOG_ERROR("Failed to initialize rendering system.");
		cleanup();
		return false;
	}

	// Initialize sound system
	LOG_INFO("Create sound system.");
	d_soundSystem.reset(SoundSystem::create());
	if (!d_soundSystem->init(config))
	{
		LOG_ERROR("Failed to initialize sound system.");
		cleanup();
		return false;
	}

	LOG_INFO("Create collision system.");
	// Initialize collision system
	// TODO Should be physics system
	d_collisionSystem.reset(new CollisionSystem());

	// Create game system
	LOG_INFO("Create game system.");
	d_gameSystem.reset(GameSystem::create());

	// Create game states
	// TODO This should be data driven
	LOG_INFO("Create game states.");
	if (!createGameStates())
	{
		LOG_ERROR("Failed to create game states.");
		cleanup();
		return false;
	}

	// Init game system
	if (!d_gameSystem->init(config)) // Init subsystem
	{
		LOG_ERROR("Failed to initialize game system.");
		cleanup();
		return false;
	}

	// Init successful
	d_init = true;
	return d_init;
}

void Engine::cleanup()
{
	d_init = false;

	// Delete game system first
	d_gameSystem = nullptr;

	// Delete sound system.
	d_soundSystem = nullptr;
		
	// Delete render system
	d_renderSystem = nullptr;

	// Collision system deleted last
	d_collisionSystem = nullptr;
}

void Engine::run()
{
	LOG_INFO("Starting game engine main loop.");
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

		updateRenderer();
		updateGame(dtime);
		updateCollision();
		swapBuffer();

		// Update time diff
		dtime = (float) glfwGetTime() - startTime;
	}
}

bool Engine::createGameStates()
{
	// Add game states
	// Load state, is initial state
	if (!d_gameSystem->addState("load", new LoadState(d_renderSystem.get(), d_collisionSystem.get(), d_soundSystem.get(), d_gameSystem.get())))
	{
		return false;
	}
	// Win state
	if (!d_gameSystem->addState("win", new WinState(d_renderSystem.get(), d_collisionSystem.get(), d_soundSystem.get())))
	{
		return false;
	}
	// Lose state
	if (!d_gameSystem->addState("lose", new LoseState(d_renderSystem.get(), d_collisionSystem.get(), d_soundSystem.get())))
	{
		return false;
	}
	// Pause state
	if (!d_gameSystem->addState("pause", new PauseState(d_renderSystem.get(), d_collisionSystem.get(), d_soundSystem.get())))
	{
		return false;
	}
	return true;
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
