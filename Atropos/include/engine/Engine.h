#pragma once

class RenderSystem; /**< Rendering subsystem forward delcare. */
class CollisionSystem; /**< Collision subsystem forward delcare. */
class GameSystem; /**< Game subsystem forward delcare. */
class SoundSystem; /**< Sound system forward declare. */

class ConfigFile; /**< Config file forward declare. */

#include "engine/IEngine.h"
#include "core/TCreatorMixin.h"

#include <memory>

/**
* \brief Game engine class.
* Provides high level initialization, game loop and cleanup.
*/
class Engine
	:
	public IEngine,
	public TCreatorMixin<Engine, IEngine>
{
public:
	/**
	* \brief Constructor.
	*/
	Engine();

	/** 
	* \brief Engine destructor.
	*/
	~Engine();

	/**
	* \brief Initializes engine and subsystems.
	* \parm config Config file name with startup data.
	* \return True on success and false on error.
	*/
	bool init(const char* configFile);

	/**
	* \brief Starts game loop.
	* The function returns after the game loop ends.
	*/
	void run();

	/**
	* \brief Explicit cleanup of resources.
	*/
	void cleanup();

protected:
	/**
	 * \brief Helper function for initializing the game system. Creates and adds game states.
	 */
	bool createGameStates();

	/**
	* \brief Update game state.
	*/
	void updateGame(float dtime);

	/**
	* \brief Collision update.
	*/
	void updateCollision();

	/**
	* \brief Renderer update.
	*/
	void updateRenderer();

	/**
	* \brief Swaps display buffers.
	* TODO: Put this into renderer?
	*/
	void swapBuffer();

private:
	bool d_init; /**< Initialization flag. */
	bool d_running; /**< Running flag. */

	std::shared_ptr<SoundSystem> d_soundSystem; /**< Sound system. */
	std::shared_ptr<RenderSystem> d_renderSystem; /**< Rendering system. */
	std::shared_ptr<CollisionSystem> d_collisionSystem; /**< Collision system. */
	std::shared_ptr<GameSystem> d_gameSystem; /**< Game system. */
};
