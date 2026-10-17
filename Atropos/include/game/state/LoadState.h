#pragma once
class IRenderSystem;
class CollisionSystem;
class ISoundSystem;
class GameSystem;

#include "game/IGameState.h"
#include "game/GameWorld.h"
#include "game/MenuShip.h"

#include <memory>
#include <vector>

/**
 * \brief Represents the loading state.
 */
class LoadState : public IGameState
{
public:
	/**
	 * \brief Sets sub systems for access.
	 */
	LoadState(IRenderSystem* renderSys, CollisionSystem* collSys, ISoundSystem* soundSys, GameSystem* gameSys);

	/**
	 * \brief Updates state.
	 */
	bool update(float dtime);

	/**
	 * \brief Initializes state.
	 */
	void init();

	/**
	 * \brief Called on entering state.
	 */
	void onEnter();

	/**
	 * \brief Called on exiting state.
	 */
	void onExit();

	/**
	 * \brief Returns string identifier of next state.
	 */
	const std::string& getNextState() const;

private:
	IRenderSystem* d_renderSystem; /**< Rendering system. */
	CollisionSystem* d_collisionSystem; /**< Collision system. */
	ISoundSystem* d_soundSystem; /**< Sound system. */
	GameSystem* d_gameSystem; /**< Game system. */
	MenuShip d_ship; /**< Model shown once the resources are loaded. */
	GameWorld d_world; /**< Game objects. */
	std::string d_nextState; /**< Next state. */
	bool d_bgmStarted; /**< True once the loading music started. */
	bool d_ready; /**< True once all resources are loaded. */
};
