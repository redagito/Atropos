#include "engine/Engine.h"

#include "renderer/RenderSystem.h"
#include "collision/CollisionSystem.h"
#include "sound/SoundSystem.h"
#include "game/GameSystem.h"

#include "game/state/LoadState.h"
#include "game/state/LoseState.h"
#include "game/state/WinState.h"
#include "game/state/PauseState.h"

#include "debug/Profiler.h"

#include "core/ThreadPool.h"

#include "util/Input.h"

#include <algorithm>

#include "io/ConfigFile.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <memory>

#include "glfw/glfw3.h"

Engine::Engine()
	:
	d_init(false),
	d_running(false),
	d_pool(nullptr),
	d_soundSystem(nullptr),
	d_renderSystem(nullptr),
	d_collisionSystem(nullptr),
	d_gameSystem(nullptr)
{
	return;
}

Engine::~Engine()
{
	cleanup();
	SPDLOG_INFO("{}", Profiler::toString());
}

void Engine::init(const char* configFile)
{
	if (d_init)
	{
		return;
	}

	ConfigFile config;
	config.load(configFile);

	// Logging is set up before any subsystem is created, so their constructors can log.
	auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
		config.get("log_filename", std::string("log.txt")), true);
	auto logger = std::make_shared<spdlog::logger>(
		"atropos", spdlog::sinks_init_list{ consoleSink, fileSink });
	logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
	logger->set_level(spdlog::level::trace);
	spdlog::set_default_logger(logger);

	// Must outlive every system that owns a loader.
	const int workerCount = config.get("engine_workerthreads", 4);
	d_pool = std::make_unique<ThreadPool>(static_cast<unsigned int>(workerCount < 1 ? 1 : workerCount));
	SPDLOG_INFO("Created worker pool with {} threads.", d_pool->getWorkerCount());

	SPDLOG_INFO("Create rendering system.");
	std::shared_ptr<RenderSystem> renderSystem(new RenderSystem);
	renderSystem->init(config, *d_pool);
	d_renderSystem = renderSystem;

	SPDLOG_INFO("Create sound system.");
	std::shared_ptr<SoundSystem> soundSystem(SoundSystem::create());
	soundSystem->init(config, *d_pool);
	d_soundSystem = soundSystem;

	SPDLOG_INFO("Create collision system.");
	// TODO Should be physics system
	d_collisionSystem.reset(new CollisionSystem());

	SPDLOG_INFO("Create game system.");
	d_gameSystem.reset(GameSystem::create());

	// TODO This should be data driven
	SPDLOG_INFO("Create game states.");
	createGameStates();

	d_gameSystem->init(config);

	d_init = true;
}

void Engine::cleanup()
{
	d_init = false;

	d_gameSystem = nullptr;

	// Stop the workers first so no in flight parse can touch a destroyed loader.
	d_pool = nullptr;

	d_soundSystem = nullptr;
		
	d_renderSystem = nullptr;

	d_collisionSystem = nullptr;
}

void Engine::run()
{
	SPDLOG_INFO("Starting game engine main loop.");
	float dtime = 0.f;

	d_running = true;

	while (!d_renderSystem->shouldClose() && d_running)
	{
		Profiler loopProfiler("engine_loop");
		float startTime = (float)glfwGetTime();

		glfwPollEvents();
		Input::poll(d_renderSystem->window());

		// F2 swaps culling; the press is edge triggered, so holding it does not repeat.
		if (Input::pressed(GLFW_KEY_F2))
		{
			d_renderSystem->setCulling(!d_renderSystem->getCulling());
		}

		if (Input::pressed(GLFW_KEY_ESCAPE))
		{
			d_renderSystem->requestClose();
		}

		updateResources();
		d_renderSystem->ui().setFrameTime(dtime);
		updateRenderer(dtime);
		updateGame(dtime);
		updateCollision();
		swapBuffer();

		// Capped so a hitch cannot step objects through each other.
		dtime = std::min((float) glfwGetTime() - startTime, MAX_DTIME);
	}
}

void Engine::createGameStates()
{
	// Load state, is initial state
	d_gameSystem->addState("load", new LoadState(d_renderSystem.get(), d_collisionSystem.get(), d_soundSystem.get(), d_gameSystem.get(), d_runStats));
	d_gameSystem->addState("win", new WinState(d_renderSystem.get(), d_collisionSystem.get(), d_soundSystem.get(), d_runStats));
	d_gameSystem->addState("lose", new LoseState(d_renderSystem.get(), d_collisionSystem.get(), d_soundSystem.get(), d_runStats));
	d_gameSystem->addState("pause", new PauseState(d_renderSystem.get(), d_collisionSystem.get(), d_soundSystem.get()));
}

void Engine::updateGame(float dtime)
{
	Profiler profiler("update_game");
	d_running = d_gameSystem->update(dtime);
}

void Engine::updateCollision()
{
	// A frozen state must stop collision too, or damage accumulates until resume.
	if (!d_gameSystem->collisionsEnabled())
	{
		return;
	}
	Profiler profiler("update_collision");
	d_collisionSystem->update();
}

void Engine::updateResources()
{
	Profiler profiler("update_resources");
	d_renderSystem->process();
	d_soundSystem->process();
}

void Engine::updateRenderer(float dtime)
{
	Profiler profiler("update_renderer");
	d_renderSystem->draw(dtime);
}

void Engine::swapBuffer()
{
	Profiler profiler("swap_buffer");
	d_renderSystem->swapBuffer();
}
