#pragma once

class RenderSystem;
class CollisionSystem;
class GameSystem;
class ISoundSystem;
class IRenderSystem;
class ThreadPool;

class ConfigFile;

#include "engine/IEngine.h"
#include "core/TCreatorMixin.h"

#include "game/RunStats.h"

#include <memory>

class Engine
	:
	public IEngine,
	public TCreatorMixin<Engine, IEngine>
{
public:
	/** \brief Frame delta cap in seconds; a longer frame can step objects through each other. */
	static constexpr float MAX_DTIME = 0.1f;

	Engine();
	~Engine();

	/** \brief Initializes engine and subsystems; throws std::runtime_error on failure. */
	void init(const char* configFile);

	void run();

	void cleanup();

protected:
	void createGameStates();
	void updateGame(float dtime);
	void updateCollision();
	void updateResources();
	void updateRenderer(float dtime);

	/** \brief Swaps display buffers. TODO: Put this into renderer? */
	void swapBuffer();

private:
	bool d_init;
	bool d_running;

	std::unique_ptr<ThreadPool> d_pool;
	std::shared_ptr<ISoundSystem> d_soundSystem;
	std::shared_ptr<IRenderSystem> d_renderSystem;
	std::shared_ptr<CollisionSystem> d_collisionSystem;
	std::shared_ptr<GameSystem> d_gameSystem;

	/** \brief Shared with the states so the end screens can show the run's score. */
	RunStats d_runStats;
};
