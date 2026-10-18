#include "game/control/RemoveOnDeathController.h"

#include "game/GameObject.h"



RemoveOnDeathController::RemoveOnDeathController()
	:
	d_object(nullptr),
	d_active(true)
{
	return;
}

RemoveOnDeathController::~RemoveOnDeathController()
{
	return;
}

void RemoveOnDeathController::attach(GameObject* object)
{
	d_object = object;
}

void RemoveOnDeathController::detach()
{
	d_object = nullptr;
}

void RemoveOnDeathController::update(float)
{
	if (d_object != nullptr && d_active && d_object->isDead())
	{
		d_object->markDeleted();
	}
}

void RemoveOnDeathController::setActive(bool state)
{
	d_active = state;
}
