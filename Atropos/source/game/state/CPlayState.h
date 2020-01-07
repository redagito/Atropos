// TODO: The sub systems should be accessible by a global communication port,
// the play state class just hands them over to the game objects.
class CRenderSystem;
class CCollisionSystem;
class CSoundSystem;
class CGameObject;

#include "game/IGameState.h"
#include "game/CGameWorld.h"

#include <memory>
#include <list>

/**
* \brief The play state represents the main gameplay.
*/
class CPlayState : public IGameState
{
public:
	/**
	* \brief Sets sub systems.
	*/
	CPlayState(CRenderSystem* renderSys, CCollisionSystem* collSys, CSoundSystem* soundSys);

	/**
	* \brief Updates state.
	*/
	bool update(float dtime);

	/**
	* \brief initializes state.
	*/
	bool init();

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
	CRenderSystem* d_renderSystem; /**< Rendering system. */
	CCollisionSystem* d_collisionSystem; /**< Collision system. */
	CSoundSystem* d_soundSystem; /**< Sound system. */

	CGameWorld d_world; /**< Game objects. */
	CGameObject* d_player; /**< Player object. */

	unsigned int d_playerGroup; /**< Player collision group. */
	unsigned int d_enemyGroup; /**< Enemy collision group. */
	unsigned int d_staticGroup; /**< Static environment collision group. */
	float d_enemySpawn; /**< Enemy spawn timer. */

	std::string d_nextState; /**< Next state. */
};
