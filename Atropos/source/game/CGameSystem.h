#pragma once

class CConfigFile; /**< Config file forward declare. */

#include "IGameState.h"

#include "core/TCreatorMixin.h"

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

/**
* \brief Manages game objects
* High level system for managing and updating the game objects and logic
*/
class CGameSystem : public TCreatorMixin<CGameSystem>
{
public:
	/**
	* \brief Default constructor.
	*/
	CGameSystem();

	/**
	* \brief Cleans up all game objects.
	*/
	~CGameSystem();

	/**
	* \brief Updates the game objects
	*/
	bool update(float dtime);
	
	/**
	* \brief Initialize game system with startup config file.
	*/
	bool init(const CConfigFile& config);

	/**
	* \brief Add game state by name and initializes the state.
	* \return True if the state could be initialized and added. False otherwise.
	* State must be heap allocated, ownership is transferred to the game system.
	*/
	bool addState(const std::string& name, IGameState* state);

	/**
	* \brief Removes state from list and deletes state object.
	*/
	void removeState(const std::string& name);

private:
	std::unordered_map<std::string, std::unique_ptr<IGameState>> d_gameStates; /**< Game states stored by name. */
	IGameState* d_activeState; /**< Current game state. */
};
