#pragma once

#include <string>

inline const std::string QUIT_STATE_NAME = "quit";

class IGameState
{
public:
	virtual ~IGameState();

	virtual void init() = 0;

	virtual void onEnter() = 0;

	/**
	* \brief Updates state.
	* \return False if state has ended, true otherwise.
	*/
	virtual bool update(float dtime) = 0;

	virtual void onExit() = 0;

	// TODO: Should this return the actual next state?
	virtual const std::string& getNextState() const = 0;

	// A state that freezes the world must return false, or collidables keep accumulating damage.
	virtual bool collisionsEnabled() const
	{
		return true;
	}
};