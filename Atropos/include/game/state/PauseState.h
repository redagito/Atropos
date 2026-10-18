#pragma once
class IRenderSystem;
class CollisionSystem;
class ISoundSystem;

#include "game/IGameState.h"
#include "game/GameWorld.h"

#include <memory>
#include <vector>

class PauseState : public IGameState
{
public:
	PauseState(IRenderSystem* renderSys, CollisionSystem* collSys, ISoundSystem* soundSys);

	bool update(float dtime);

	void init();

	void onEnter();

	void onExit();

	const std::string& getNextState() const;

	/**
	* \brief Pausing freezes the world, so collisions must stop as well.
	*/
	bool collisionsEnabled() const;

private:
	IRenderSystem* d_renderSystem; /**< Rendering system. */
	CollisionSystem* d_collisionSystem; /**< Collision system. */
	ISoundSystem* d_soundSystem; /**< Sound system. */
	GameWorld d_world; /**< Game objects. */
	std::string d_nextState; /**< Next state. */
};
