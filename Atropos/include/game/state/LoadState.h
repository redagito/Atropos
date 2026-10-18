#pragma once
class IRenderSystem;
class CollisionSystem;
class ISoundSystem;
class GameSystem;

#include "game/IGameState.h"
#include "game/GameWorld.h"
#include "game/MenuShip.h"
#include "game/RunStats.h"

#include <memory>
#include <vector>

class LoadState : public IGameState
{
public:
	LoadState(IRenderSystem* renderSys, CollisionSystem* collSys, ISoundSystem* soundSys, GameSystem* gameSys, RunStats& runStats);

	bool update(float dtime);

	void init();

	void onEnter();

	void onExit();

	const std::string& getNextState() const;

private:
	IRenderSystem* d_renderSystem; /**< Rendering system. */
	CollisionSystem* d_collisionSystem; /**< Collision system. */
	ISoundSystem* d_soundSystem; /**< Sound system. */
	GameSystem* d_gameSystem; /**< Game system. */
	RunStats& d_runStats; /**< Score of the run being started. */
	MenuShip d_ship; /**< Model shown once the resources are loaded. */
	GameWorld d_world; /**< Game objects. */
	std::string d_nextState; /**< Next state. */
	bool d_bgmStarted; /**< True once the loading music started. */
	bool d_ready; /**< True once all resources are loaded. */
};
