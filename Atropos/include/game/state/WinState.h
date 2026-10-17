#pragma once
class IRenderSystem;
class CollisionSystem;
class ISoundSystem;

#include "game/IGameState.h"
#include "game/GameWorld.h"
#include "game/MenuShip.h"

#include <memory>
#include <vector>

/**
* \brief Represents the win state.
*/
class WinState : public IGameState
{
public:
	/**
	* \brief Sets sub systems for access.
	*/
	WinState(IRenderSystem* renderSys, CollisionSystem* collSys, ISoundSystem* soundSys);

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
	MenuShip d_ship; /**< Model shown on the win screen. */
	GameWorld d_world; /**< Game objects. */
	std::string d_nextState; /**< Next state. */
};
