#include "CRemoveOnDeathController.h"

#include "game/CGameObject.h"



CRemoveOnDeathController::CRemoveOnDeathController()
	:
	d_object(nullptr),
	d_active(true)
{
	return;
}

CRemoveOnDeathController::~CRemoveOnDeathController()
{
	return;
}

void CRemoveOnDeathController::attach(CGameObject* object)
{
	d_object = object;
}

void CRemoveOnDeathController::detach()
{
	d_object = nullptr;
}

void CRemoveOnDeathController::update(float dtime)
{
	if (d_object != nullptr && d_active && d_object->isDead())
	{
		// Mark for deletion
		d_object->markDeleted();
	}
}

void CRemoveOnDeathController::setActive(bool state)
{
	d_active = state;
}
