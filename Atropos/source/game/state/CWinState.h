class CRenderSystem;
class CCollisionSystem;
class CSoundSystem;

#include "game/IGameState.h"
#include "game/CGameWorld.h"

#include <memory>
#include <vector>

/**
* \brief Represents the loading state.
*/
class CWinState : public IGameState
{
public:
	/**
	* \brief Sets sub systems for access.
	*/
	CWinState(CRenderSystem* renderSys, CCollisionSystem* collSys, CSoundSystem* soundSys);

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
	CRenderSystem* d_renderSystem; /**< Rendering system. */
	CCollisionSystem* d_collisionSystem; /**< Collision system. */
	CSoundSystem* d_soundSystem; /**< Sound system. */
	CGameWorld d_world; /**< Game objects. */
	std::string d_nextState; /**< Next state. */
};
