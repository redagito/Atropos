#include "game/control/ExplodeOnDeathController.h"

#include "game/GameObject.h"
#include "sound/SoundSystem.h"



ExplodeOnDeathController::ExplodeOnDeathController(ISoundSystem* soundSystem, const std::string& soundName)
	:
	d_object(nullptr),
	d_soundSystem(soundSystem),
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
		d_exploded = true;
	}
}

void ExplodeOnDeathController::setActive(bool state)
{
	d_active = state;
}
