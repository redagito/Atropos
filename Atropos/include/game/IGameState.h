#pragma once

#include <string>

/**
* \brief Next-state name that requests the engine to stop.
* A state returns false from update() with this as its next state to quit.
*/
inline const std::string QUIT_STATE_NAME = "quit";

/**
* \brief Game state interface class.
* A game state represents a high level state in a game.
* Several states can be chained together to form the high level game flow. 
*/
class IGameState
{
public:
	/**
	* \brief Virtual destructor.
	*/
	virtual ~IGameState();

	/**
	* \brief Explicit initialization.
	* Throws std::runtime_error if the state cannot be initialized.
	*/
	virtual void init() = 0;

	/**
	* \brief Called on entering state.
	*/
	virtual void onEnter() = 0;

	/**
	* \brief Updates state.
	* \return False if state has ended, true otherwise.
	*/
	virtual bool update(float dtime) = 0;

	/**
	* \brief Called on exiting state.
	*/
	virtual void onExit() = 0;

	/**
	* \brief Returns requested state transition.
	* TODO: Should this return the actual next state?
	*/
	virtual const std::string& getNextState() const = 0;

	/**
	* \brief True while collisions should be processed.
	* A state that freezes the world must return false, otherwise damage keeps
	* accumulating in the collidables while nothing consumes it.
	*/
	virtual bool collisionsEnabled() const
	{
		return true;
	}
};