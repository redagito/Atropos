#pragma once
class IRenderSystem;
class CollisionSystem;
class ISoundSystem;


#include "game/IGameState.h"
#include "game/RunStats.h"
#include "game/GameWorld.h"
#include "game/MenuShip.h"

#include <memory>
#include <vector>

class WinState : public IGameState
{
public:
	WinState(IRenderSystem* renderSys, CollisionSystem* collSys, ISoundSystem* soundSys, RunStats& runStats);

	bool update(float dtime);

	void init();

	void onEnter();

	void onExit();

	const std::string& getNextState() const;

private:
	IRenderSystem* d_renderSystem; /**< Rendering system. */
	CollisionSystem* d_collisionSystem; /**< Collision system. */
	ISoundSystem* d_soundSystem; /**< Sound system. */
	RunStats& d_runStats; /**< Score of the run that just ended. */
	MenuShip d_ship; /**< Model shown on the win screen. */
	GameWorld d_world; /**< Game objects. */
	std::string d_nextState; /**< Next state. */
};
