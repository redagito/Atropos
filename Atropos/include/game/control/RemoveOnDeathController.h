#pragma once

#include "game/IController.h"

#include <string>

/**
* \brief Removes the object when it enters the death state.
* TODO This should actually react to an onDeath event.
* TODO This actually can play any sound on death.
*/
class RemoveOnDeathController : public IController
{
public:
	/**
	* \brief Sets sound system and sound name.
	*/
	RemoveOnDeathController();

	/**
	* \brief Destructor for cleanup.
	*/
	~RemoveOnDeathController();

	/**
	* \brief On attach to object.
	*/
	void attach(GameObject* object);

	/**
	* \brief On detaching from game object.
	*/
	void detach();

	/**
	* \brief Updates controller.
	*/
	void update(float dtime);

	void setActive(bool active);

private:
	GameObject* d_object; /**< Controlled game object. */
	bool d_active;
};
