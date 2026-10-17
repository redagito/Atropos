#pragma once
// TODO: The sub systems should be accessible by a global communication port,
// the play state class just hands them over to the game objects.
class IRenderSystem;
class CollisionSystem;
class ISoundSystem;
class GameObject;

#include "game/IGameState.h"
#include "game/GameWorld.h"

#include <memory>
#include <list>

/**
* \brief The play state represents the main gameplay.
*/
class PlayState : public IGameState
{
public:
	/**
	* \brief Sets sub systems.
	*/
	PlayState(IRenderSystem* renderSys, CollisionSystem* collSys, ISoundSystem* soundSys);

	/**
	* \brief Destroys the world and releases the collision groups.
	*/
	~PlayState();

	/**
	* \brief Updates state.
	*/
	bool update(float dtime);

	/**
	* \brief initializes state.
	*/
	void init();

	/**
	* \brief On state enter callback.
	*/
	void onEnter();
	
	/**
	* \brief On state exit callback.
	*/
	void onExit();

	/**
	* \brief Returns next state name.
	*/
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
