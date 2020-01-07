#include "CExplodeOnDeathController.h"

#include "game/CGameObject.h"
#include "collision/CCollidable.h"
#include "sound/CSoundSystem.h"



CExplodeOnDeathController::CExplodeOnDeathController(CSoundSystem* soundSystem, const std::string& soundName)
	:
	d_object(nullptr),
	d_soundSystem(soundSystem),
	d_soundName(soundName),
	d_active(true)
{
	return;
}

CExplodeOnDeathController::~CExplodeOnDeathController()
{
	return;
}

void CExplodeOnDeathController::attach(CGameObject* object)
{
	d_object = object;
}

void CExplodeOnDeathController::detach()
{
	d_object = nullptr;
}

void CExplodeOnDeathController::update(float dtime)
{
	if (d_object != nullptr && d_active && d_object->isDead())
	{
		// Trigger explosion
		d_soundSystem->playSFX(d_soundName);
	}
}

void CExplodeOnDeathController::setActive(bool state)
{
	d_active = state;
}
