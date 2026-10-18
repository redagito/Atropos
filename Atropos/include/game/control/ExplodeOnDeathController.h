#pragma once

class ISoundSystem;
class IRenderSystem;

#include "game/IController.h"

#include <string>

/**
* \brief Plays a sound and starts a fireball burst the one time its object dies.
*/
class ExplodeOnDeathController : public IController
{
public:
	ExplodeOnDeathController(ISoundSystem* soundSystem, const std::string& soundName, IRenderSystem* renderSystem);

	~ExplodeOnDeathController();

	void attach(GameObject* object);

	void detach();

	void update(float dtime);

	void setActive(bool active);

private:
	GameObject* d_object; /**< Controlled game object. */
	ISoundSystem* d_soundSystem; /**< Sound system. */
	IRenderSystem* d_renderSystem; /**< Render system, which owns the explosion effect. */
	std::string d_soundName; /**< Sound name to be played. */
	bool d_active;
	bool d_exploded; /**< Whether the explosion was already triggered. */
};
