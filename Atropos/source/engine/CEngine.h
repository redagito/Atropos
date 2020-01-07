#pragma once

class CRenderSystem; /**< Rendering subsystem forward delcare. */
class CCollisionSystem; /**< Collision subsystem forward delcare. */
class CGameSystem; /**< Game subsystem forward delcare. */
class CSoundSystem; /**< Sound system forward declare. */

class CConfigFile; /**< Config file forward declare. */

#include "IEngine.h"
#include "core/TCreatorMixin.h"
#include "core/job/CJobSystem.h"

#include <memory>

/**
* \brief Game engine class.
* Provides high level initialization, game loop and cleanup.
*/
class CEngine
	:
	public IEngine, // Engine interface
	public TCreatorMixin<CEngine, IEngine> // Creator mixin
{
public:
	/**
	* \brief Constructor.
	*/
	CEngine();

	/** 
	* \brief Engine destructor.
	*/
	~CEngine();

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

	std::shared_ptr<CSoundSystem> d_soundSystem; /**< Sound system. */
	std::shared_ptr<CRenderSystem> d_renderSystem; /**< Rendering system. */
	std::shared_ptr<CCollisionSystem> d_collisionSystem; /**< Collision system. */
	std::shared_ptr<CGameSystem> d_gameSystem; /**< Game system. */
	CJobSystem d_jobSystem; /**< Job system singleton. */
};
