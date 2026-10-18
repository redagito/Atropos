#include "game/control/ExplodeOnDeathController.h"

#include "game/GameObject.h"
#include "renderer/IRenderSystem.h"
#include "sound/ISoundSystem.h"

#include <glm/ext.hpp>



ExplodeOnDeathController::ExplodeOnDeathController(ISoundSystem* soundSystem, const std::string& soundName, IRenderSystem* renderSystem)
	:
	d_object(nullptr),
	d_soundSystem(soundSystem),
	d_renderSystem(renderSystem),
	d_soundName(soundName),
	d_active(true),
	d_exploded(false)
{
	return;
}

ExplodeOnDeathController::~ExplodeOnDeathController()
{
	return;
}

void ExplodeOnDeathController::attach(GameObject* object)
{
	d_object = object;
}

void ExplodeOnDeathController::detach()
{
	d_object = nullptr;
}

void ExplodeOnDeathController::update(float)
{
	if (d_object != nullptr && d_active && d_object->isDead() && !d_exploded)
	{
		d_soundSystem->playSFX(d_soundName);

		// The explosion has to be placed before the world erases the object.
		const glm::vec4 point = d_object->getTranslation() * glm::vec4(0.f, 0.f, 0.f, 1.f);
		d_renderSystem->addExplosion(glm::vec3(point.x / point.w, point.y / point.w, point.z / point.w));

		d_exploded = true;
	}
}

void ExplodeOnDeathController::setActive(bool state)
{
	d_active = state;
}
