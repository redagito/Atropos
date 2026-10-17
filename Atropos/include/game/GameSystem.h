#pragma once

class ConfigFile; /**< Config file forward declare. */

#include "game/IGameState.h"

#include "core/TCreatorMixin.h"

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

/**
* \brief Manages game objects
* High level system for managing and updating the game objects and logic
*/
class GameSystem : public TCreatorMixin<GameSystem>
{
public:
	/**
	* \brief Default constructor.
	*/
	GameSystem();

	/**
	* \brief Cleans up all game objects.
	*/
	~GameSystem();

	/**
	* \brief Updates the game objects
	*/
	bool update(float dtime);
	
	/**
	* \brief Initialize game system with startup config file.
	* Throws std::runtime_error if the initial state is not registered.
	*/
	void init(const ConfigFile& config);

	/**
	* \brief Add game state by name and initializes the state.
	* State must be heap allocated, ownership is transferred to the game system.
	* Throws std::runtime_error if the state cannot be initialized.
	*/
	void addState(const std::string& name, IGameState* state);

	/**
	* \brief Removes state from list and deletes state object.
	*/
	void removeState(const std::string& name);

	/**
	* \brief True while the active state wants collisions to be processed.
	*/
	bool collisionsEnabled() const;

private:
	std::unordered_map<std::string, std::unique_ptr<IGameState>> d_gameStates; /**< Game states stored by name. */
	IGameState* d_activeState; /**< Current game state. */
};
