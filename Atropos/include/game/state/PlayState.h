#pragma once
// TODO: Sub systems should come from a global port rather than constructor arguments.
class IRenderSystem;
class CollisionSystem;
class ISoundSystem;
class GameObject;

#include "game/IGameState.h"
#include "game/GameWorld.h"

#include <memory>
#include <list>

class PlayState : public IGameState
{
public:
	PlayState(IRenderSystem* renderSys, CollisionSystem* collSys, ISoundSystem* soundSys);

	~PlayState();

	bool update(float dtime);

	void init();

	void onEnter();
	
	void onExit();

	const std::string& getNextState() const;

private:
	IRenderSystem* d_renderSystem; /**< Rendering system. */
	CollisionSystem* d_collisionSystem; /**< Collision system. */
	ISoundSystem* d_soundSystem; /**< Sound system. */

	GameWorld d_world; /**< Game objects. */
	GameObject* d_player; /**< Player object. */

	unsigned int d_playerGroup; /**< Player collision group. */
	unsigned int d_enemyGroup; /**< Enemy collision group. */
	float d_enemySpawn; /**< Enemy spawn timer. */

	std::string d_nextState; /**< Next state. */
};
