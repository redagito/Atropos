#pragma once

class ISoundSystem;

#include "game/IController.h"

#include <string>

// Despite the name, this plays a sound and creates no explosion effect.
// TODO: Should react to an onDeath event.
class ExplodeOnDeathController : public IController
{
public:
	ExplodeOnDeathController(ISoundSystem* soundSystem, const std::string& soundName);

	~ExplodeOnDeathController();

	void attach(GameObject* object);

	void detach();

	void update(float dtime);

	void setActive(bool active);

private:
	GameObject* d_object; /**< Controlled game object. */
	ISoundSystem* d_soundSystem; /**< Sound system. */
	std::string d_soundName; /**< Sound name to be played. */
	bool d_active;
	bool d_exploded; /**< Whether the explosion was already triggered. */
};
