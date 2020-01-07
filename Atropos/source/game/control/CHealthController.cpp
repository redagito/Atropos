#include "CHealthController.h"

#include "game/CGameObject.h"
#include "collision/CCollidable.h"



CHealthController::CHealthController(float health)
	:
	d_health(health),
	d_object(nullptr),
	d_active(true)
{
	return;
}

CHealthController::~CHealthController()
{
	return;
}

void CHealthController::attach(CGameObject* object)
{
	d_object = object;
}

void CHealthController::detach()
{
	d_object = nullptr;
}

void CHealthController::update(float dtime)
{
	if (d_active && d_object != nullptr)
	{
		// Set health
		d_health -= d_object->getCollidable()->getDamageReceived();
		if (d_health <= 0.f)
		{
			// Set object into death state
			d_object->setDead();
		}
	}
}

void CHealthController::setActive(bool state)
{
	d_active = state;
}