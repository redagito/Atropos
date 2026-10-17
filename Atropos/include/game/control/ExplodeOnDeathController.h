#pragma once

class SoundSystem;

#include "game/IController.h"

#include <string>

/**
* \brief Checks the object state for death state and trigger explosion effect.
* TODO This should actually react to an onDeath event.
* TODO This actually can play any sound on death.
*/
class ExplodeOnDeathController : public IController
{
public:
	/**
	* \brief Sets sound system and sound name.
	*/
	ExplodeOnDeathController(SoundSystem* soundSystem, const std::string& soundName);

	/**
	* \brief Destructor for cleanup.
	*/
	~ExplodeOnDeathController();

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
	SoundSystem* d_soundSystem; /**< Sound system. */
	std::string d_soundName; /**< Sound name to be played. */
	bool d_active;
	bool d_exploded; /**< Whether the explosion was already triggered. */
};
