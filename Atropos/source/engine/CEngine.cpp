#include "CEngine.h"

// Sub systems
#include "renderer/CRenderSystem.h"
#include "collision/CCollisionSystem.h"
#include "sound/CSoundSystem.h"
#include "game/CGameSystem.h"

// Game states
#include "game/state/CLoadState.h"
#include "game/state/CLoseState.h"
#include "game/state/CWinState.h"
#include "game/state/CPauseState.h"

// Debug and profiling
#include "debug/Log.h"
#include "debug/CLogger.h"
#include "debug/CProfiler.h"

// IO and utility
#include "io/CConfigFile.h"
#include "util/CTimeStamp.h"
#include "util/Global.h"

#include "renderer/flext/flextGL.h"

#include "glfw/glfw3.h"
#include "glm/ext.hpp"

#include <cmath>
#include <iostream>
#include <list>
#include <cassert>

CEngine::CEngine()
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

CEngine::~CEngine()
{
	cleanup();
	// Log profiler data
	LOG_INFO("%s", CProfiler::toString().c_str());
}

bool CEngine::init(const char* configFile)
{
	// Prevent multiple init calls
	if (d_init)
	{
		return true;
	}

	// Configuration file with startup config
	CConfigFile config;
	bool configLoaded = config.load(configFile);

	// Init log file before checking config load state
	CLogger::initLogFile(config.get("log_filename", std::string("log/") + CTimeStamp::createTimeStamp() + ".log"));

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
	d_renderSystem.reset(new CRenderSystem);
	if (!d_renderSystem->init(config))
	{
		LOG_ERROR("Failed to initialize rendering system.");
		cleanup();
		return false;
	}

	// Initialize sound system
	LOG_INFO("Create sound system.");
	d_soundSystem.reset(CSoundSystem::create());
	if (!d_soundSystem->init(config))
	{
		LOG_ERROR("Failed to initialize sound system.");
		cleanup();
		return false;
	}

	LOG_INFO("Create collision system.");
	// Initialize collision system
	// TODO Should be physics system
	d_collisionSystem.reset(new CCollisionSystem());

	// Create game system
	LOG_INFO("Create game system.");
	d_gameSystem.reset(CGameSystem::create());

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

void CEngine::cleanup()
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

void CEngine::run()
{
	LOG_INFO("Starting game engine main loop.");
	// Time difference
	float dtime = 0.f;
	float accumTime = 0.f;
	unsigned int frameCount = 0;
	float f2Reset = 0.f;
	float lastShot = 0.f;

	// Main loop
	d_running = true;

	while (!glfwWindowShouldClose(Global::window) && d_running)
	{
		CProfiler loopProfiler("engine_loop");
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
		
		accumTime += dtime;
		f2Reset += dtime;
		if (accumTime >= 1.f)
		{
			accumTime -= 1.f;
			// Print frame count
			LOG_DEBUG("FPS: %u", frameCount);
			// Print rendering statistics
			LOG_DEBUG("%s", d_renderSystem->statisticsToString().c_str());
			frameCount = 0;
		}
		++frameCount;
		
		updateRenderer();
		updateGame(dtime);
		updateCollision();
		swapBuffer();

		// Update time diff
		dtime = (float) glfwGetTime() - startTime;
	}
}

bool CEngine::createGameStates()
{
	// Add game states
	// Load state, is initial state
	if (!d_gameSystem->addState("load", new CLoadState(d_renderSystem.get(), d_collisionSystem.get(), d_soundSystem.get(), d_gameSystem.get())))
	{
		return false;
	}
	// Win state
	if (!d_gameSystem->addState("win", new CWinState(d_renderSystem.get(), d_collisionSystem.get(), d_soundSystem.get())))
	{
		return false;
	}
	// Lose state
	if (!d_gameSystem->addState("lose", new CLoseState(d_renderSystem.get(), d_collisionSystem.get(), d_soundSystem.get())))
	{
		return false;
	}
	// Pause state
	if (!d_gameSystem->addState("pause", new CPauseState(d_renderSystem.get(), d_collisionSystem.get(), d_soundSystem.get())))
	{
		return false;
	}
	return true;
}

void CEngine::updateGame(float dtime)
{
	// Update game logic
	CProfiler profiler("update_game");
	d_running = d_gameSystem->update(dtime);
}

void CEngine::updateCollision()
{
	// Update collisions
	CProfiler profiler("update_collision");
	d_collisionSystem->update();
}

void CEngine::updateRenderer()
{
	// Draw the scene
	CProfiler profiler("update_renderer");
	d_renderSystem->draw();
}

void CEngine::swapBuffer()
{
	// Finish draw and swap buffer
	CProfiler profiler("swap_buffer");
	d_renderSystem->swapBuffer();
}
