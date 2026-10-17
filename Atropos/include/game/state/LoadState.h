#pragma once
class RenderSystem;
class CollisionSystem;
class SoundSystem;
class GameSystem;

#include "game/IGameState.h"
#include "game/GameWorld.h"

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
	LoadState(RenderSystem* renderSys, CollisionSystem* collSys, SoundSystem* soundSys, GameSystem* gameSys);

	/**
	 * \brief Updates state.
	 */
	bool update(float dtime);

	/**
	 * \brief Initializes state.
	 */
	bool init();

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
	RenderSystem* d_renderSystem; /**< Rendering system. */
	CollisionSystem* d_collisionSystem; /**< Collision system. */
	SoundSystem* d_soundSystem; /**< Sound system. */
	GameSystem* d_gameSystem; /**< Game system. */
	GameWorld d_world; /**< Game objects. */
	std::string d_nextState; /**< Next state. */
};
