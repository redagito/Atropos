#pragma once
// TODO: Sub systems should come from a global port rather than constructor arguments.
class IRenderSystem;
class CollisionSystem;
class ISoundSystem;
class GameObject;
class HealthController;

#include "game/IGameState.h"
#include "game/GameWorld.h"
#include "game/RunStats.h"

#include <memory>
#include <list>

class PlayState : public IGameState
{
public:
	/** \brief Camera travel that ends the run; also drives the progress readout. */
	static constexpr float GOAL_DISTANCE = 8000.f;

	PlayState(IRenderSystem* renderSys, CollisionSystem* collSys, ISoundSystem* soundSys, RunStats& runStats);

	~PlayState();

	bool update(float dtime);

	void init();

	void onEnter();
	
	void onExit();

	const std::string& getNextState() const;

private:
	/** \brief Draws the health bar, progress and score. */
	void drawInterface();

	IRenderSystem* d_renderSystem; /**< Rendering system. */
	CollisionSystem* d_collisionSystem; /**< Collision system. */
	ISoundSystem* d_soundSystem; /**< Sound system. */

	GameWorld d_world; /**< Game objects. */
	GameObject* d_player; /**< Player object. */
	std::shared_ptr<HealthController> d_playerHealth; /**< Kept to read health for the interface. */
	RunStats& d_runStats; /**< Score of this run. */

	unsigned int d_playerGroup; /**< Player collision group. */
	unsigned int d_enemyGroup; /**< Enemy collision group. */
	float d_enemySpawn; /**< Enemy spawn timer. */

	std::string d_nextState; /**< Next state. */
};
